/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "telepathyserver.h"
#include "telepathygameinfo.h"
#include "telepathyhandler.h"
#include "../serveradapter.h"

#include <QTcpSocket>
#include <TelepathyQt4/PendingStreamTubeConnection>

TelepathyServer::TelepathyServer(QObject *parent) :
    AbstractServer(parent)
{
    // Register to handler
    TelepathyHandler::instance()->setServer(this);
}

GameInfoModel *TelepathyServer::discoverGames() {
    return new TelepathyGameInfoModel(this);
}

void TelepathyServer::enable() {
    TelepathyHandler *handler = TelepathyHandler::instance();
    handler->setPublishInGame(true);
}

void TelepathyServer::disable() {
    TelepathyHandler *handler = TelepathyHandler::instance();
    handler->setPublishInGame(false);

    foreach(QTcpSocket *device, streamMap.keys())
        if (serverAdapter()->hasClient(device))
            serverAdapter()->removeClient(device);
        else {
            // The client does not have to be in the client list yet as it is only
            // added as soon as we have connected to the address provided by
            // Telepathy. In this case we have to remove the socket manually.
            device->close();
            device->deleteLater();
            removeClient(device);
        }
}

void TelepathyServer::handleIncomingStream(
        Tp::IncomingStreamTubeChannelPtr incomingStream) {

    if (incomingStream->tubeState() != Tp::TubeChannelStateLocalPending) {
        incomingStream->requestClose();
        return;
    }

    Tp::PendingStreamTubeConnection *connection =
            incomingStream->acceptTubeAsTcpSocket();
    connect(connection,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onConnectionEstablished(Tp::PendingOperation*)));
}

void TelepathyServer::onConnectionEstablished(Tp::PendingOperation *operation) {
    Tp::IncomingStreamTubeChannelPtr
            incomingStream = Tp::IncomingStreamTubeChannelPtr::dynamicCast(operation->object());

    if (operation->isError()) {
        qWarning() << "Failed to establish stream tube: "
                   << operation->errorMessage();

        if (incomingStream)
            incomingStream->requestClose();
        return;
    }

    // Cast to Tp::StreamTubeChannelPtr to work around a bug in Telepathy Qt
    // < 0.7.2
    QPair<QHostAddress, quint16> address =
            Tp::StreamTubeChannelPtr::dynamicCast(incomingStream)->ipAddress();
    // Q_ASSERT(address.first.isNull());

    QTcpSocket *socket = new QTcpSocket(this);
    streamMap[socket] = incomingStream;

    connect(socket, SIGNAL(connected()), SLOT(onSocketConnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(onSocketError(QAbstractSocket::SocketError)));

    qDebug() << "Connecting to socket at: " << address.first << ":"
             << address.second;
    socket->connectToHost(address.first, address.second);
}

void TelepathyServer::onSocketConnected() {
    qDebug() << "Received connection from remote host.";

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket);

    ClientInfo *client = serverAdapter()->addClient(socket);
    connect(client, SIGNAL(removed()), SLOT(onClientRemoved()));
}

void TelepathyServer::onSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error)

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket);

    if (serverAdapter()->hasClient(socket))
        serverAdapter()->removeClient(socket);
    else {
        // The client does not have to be in the client list yet as it is only
        // added as soon as we have connected to the address provided by
        // Telepathy. In this case we have to remove the socket manually.
        socket->close();
        socket->deleteLater();
        removeClient(socket);
    }
}

void TelepathyServer::onClientRemoved() {
    ClientInfo *client = qobject_cast<ClientInfo *>(sender());
    Q_ASSERT(client);

    removeClient((QTcpSocket *) client->device());
}

void TelepathyServer::removeClient(QTcpSocket *device) {
    disconnect(device, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    disconnect(device, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(onSocketError(QAbstractSocket::SocketError)));

    Tp::IncomingStreamTubeChannelPtr incomingStream = streamMap.take(device);

    if (!incomingStream)
        return;

    incomingStream->requestClose();
}
