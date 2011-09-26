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

#include "telepathyclient.h"
#include "telepathygameinfo.h"
#include "telepathyhandler.h"

#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/ContactManager>

#include <QTcpServer>
#include <QTcpSocket>

TelepathyClient::TelepathyClient(QObject *parent) :
    AbstractClient(parent), server(NULL) {

    // Register to handler
    TelepathyHandler::instance()->setClient(this);

    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(onNewConnection()));
}

void TelepathyClient::join(GameInfo *game) {
    TelepathyGameInfo *gameInfo = qobject_cast<TelepathyGameInfo *>(game);

    if (gameInfo == NULL) {
        qDebug() << "Unsupported game parameter.";
        return;
    }

    if (!gameInfo->contact || !gameInfo->account)
        return;

    setState(Connecting);

    // Subscribe to presence changes of the remote contact - we need to disconnect if its state changes to offline
    remoteContact = gameInfo->contact;
    connect(remoteContact.data(), SIGNAL(presenceChanged(Tp::Presence)),
            SLOT(onContactPresenceChanged(Tp::Presence)));

    Tp::PendingChannelRequest *request =
            gameInfo->account->createStreamTube(gameInfo->contact, "x-sudoku-united");
    connect(request, SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onCreateTubeFinished(Tp::PendingOperation*)));

    qDebug() << "Connecting...";
}

void TelepathyClient::leave() {
    setState(Disconnected);
}

void TelepathyClient::onCreateTubeFinished(Tp::PendingOperation *op) {
    if (op->isError()) {
        setError(op->errorMessage());
        setState(Disconnected);
        return;
    }
}

void TelepathyClient::handleOutgoingStream(Tp::OutgoingStreamTubeChannelPtr stream) {
    qDebug() << "Handling outgoing stream...";

    outgoingStream = stream;
    Q_ASSERT(outgoingStream);

    if (!server->listen(QHostAddress::LocalHost)) {
        setError(server->errorString());
        setState(Disconnected);

        return;
    }

    // Listen if the remote end leaves the StreamTube
    connect(outgoingStream.data(),
            SIGNAL(connectionClosed(uint,QString,QString)),
            SLOT(onConnectionClosed(uint,QString,QString)));

    Tp::PendingOperation *op = outgoingStream->offerTcpSocket(server,
                                                              QVariantMap());
    connect(op, SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onOfferServerFinished(Tp::PendingOperation*)));
}

void TelepathyClient::onOfferServerFinished(Tp::PendingOperation *operation) {
    qDebug() << "Offered server to remote host";

    if (operation->isError()) {
        setError(operation->errorMessage());
        setState(Disconnected);
        return;
    }

    startTimeoutTimer();
}

/**
  * This function is called after a connection has been made to the local
  * server which signals that the connection to the remote host was
  * successfully established.
  */
void TelepathyClient::onNewConnection() {
    qDebug() << "Received new connection";

    QTcpSocket *socket = server->nextPendingConnection();

    if (device()) {
        // We already have a primary client ignore the connection
        socket->close();
        return;
    }

    setDevice(socket);
}

void TelepathyClient::onConnectionClosed(uint connectionId,
                                         const QString &errorName,
                                         const QString &errorMessage) {
    Q_UNUSED(connectionId);
    Q_UNUSED(errorName);
    Q_UNUSED(errorMessage);

    qDebug() << "Connection to remote host was closed: " << errorName
             << errorMessage;

    setError(errorMessage);
    setState(Disconnected);
}

void TelepathyClient::onContactPresenceChanged(const Tp::Presence &presence) {
    if (presence.type() == Tp::ConnectionPresenceTypeOffline)
        setState(Disconnected);
}

void TelepathyClient::setState(State state) {
    if (state == this->state())
        return;

    AbstractClient::setState(state);

    if (state == Disconnected) {
        if (outgoingStream) {
            outgoingStream->requestClose();
            outgoingStream.reset();
        }

        if (server->isListening())
            server->close();

        if (device())
            device()->deleteLater();
    }
}
