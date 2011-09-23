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

#include "bluetoothmultiplayeradapter.h"
#include "game.h"
#include "sudoku.h"

#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceInfo>
#include <QRfcommServer>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothLocalDevice>

const QLatin1String BluetoothMultiplayerAdapter::ServiceUuid = QLatin1String("614b8c48-c0af-4d20-b4d7-a24dfcd9900e");

BluetoothGameInfo::BluetoothGameInfo(QObject *parent) :
    GameInfo(parent) {

}

QString BluetoothGameInfo::name() const {
     return info.device().name();
}

quint16 BluetoothGameInfo::playerCount() const {
    return 0;
}

BluetoothMultiplayerAdapter::BluetoothMultiplayerAdapter(Sudoku *parent) :
    MultiplayerAdapter(parent), server(NULL)
{
    localBluetoothDevice = new QBluetoothLocalDevice(this);

    discoveryAgent = new QBluetoothServiceDiscoveryAgent(this);
    discoveryAgent->setUuidFilter(QBluetoothUuid(BluetoothMultiplayerAdapter::ServiceUuid));

    // Listen for game changes as we need to subscribe to the players changed property
    connect(parent, SIGNAL(gameChanged()), SLOT(onGameChanged()));
}

BluetoothMultiplayerAdapter::~BluetoothMultiplayerAdapter() {
    localBluetoothDevice->setHostMode(previousHostMode);
}

bool BluetoothMultiplayerAdapter::hostSupportsBluetooth() {
    return QBluetoothLocalDevice::allDevices().size() > 0;
}

GameInfoModel *BluetoothMultiplayerAdapter::discoverGames() {
    return new BluetoothGameInfoModel(discoveryAgent, this);
}

bool BluetoothMultiplayerAdapter::canJoinGameInfo(GameInfo *game) const {
    return qobject_cast<BluetoothGameInfo *>(game) != NULL;
}

void BluetoothMultiplayerAdapter::join(GameInfo *game) {
    BluetoothGameInfo *gameInfo = qobject_cast<BluetoothGameInfo *>(game);

    if (gameInfo == NULL)
        return;

    MultiplayerAdapter::join(game);

    if (discoveryAgent->isActive())
        discoveryAgent->stop();

    setupLocalSocket();

    qDebug() << "Connecting to service" << gameInfo->info.device().address().toString() << gameInfo->info.serverChannel();

    ((QBluetoothSocket *) localDevice())->connectToService(gameInfo->info);
}

void BluetoothMultiplayerAdapter::setupLocalSocket() {
    // Completely reset the Bluetooth socket as there have been segmentation faults when
    // reusing the old one.
    QBluetoothSocket *socket = (QBluetoothSocket *) localDevice();

    if (socket)
        socket->deleteLater();

    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);

    connect(socket, SIGNAL(connected()), SLOT(onConnectedToServer()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), SLOT(onLocalSocketError(QBluetoothSocket::SocketError)));

    setLocalDevice(socket);
}

void BluetoothMultiplayerAdapter::onLocalSocketError(QBluetoothSocket::SocketError error) {
    Q_UNUSED(error);

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    Q_ASSERT(socket);

    disconnectLocalDevice();
}

void BluetoothMultiplayerAdapter::onRemoteSocketError(QBluetoothSocket::SocketError error) {
    Q_UNUSED(error);

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    Q_ASSERT(socket);

    qDebug() << "Socket error from " << socket << "(" << socket->errorString() << ")";

    disconnectRemoteClient(socket);
}


void BluetoothMultiplayerAdapter::startServer() {
    Q_ASSERT(!server);

    server = new QRfcommServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(onNewRemoteConnection()));

    // There seems to be a problem if channel id 1 is used so we
    // force a port >= 2.
    quint16 port = 2;
    while (!server->listen(QBluetoothAddress(), port) && port < 24)
        port++;

    if (!server->isListening()) {
        qWarning() << "Failed to start Bluetooth server.";
        return;
    }

    // Heavily based on Qt Mobility's Bluetooth chat sample
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

    //! [Class Uuuid must contain at least 1 entry]
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(BluetoothMultiplayerAdapter::ServiceUuid));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
    //! [Class Uuuid must contain at least 1 entry]

    //! [Service name, description and provider]
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Sudoku United Game Server"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("A multiplayer server for the Sudoku United game."));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr(""));
    //! [Service name, description and provider]

    //! [Service UUID set]
    serviceInfo.setServiceUuid(QBluetoothUuid(BluetoothMultiplayerAdapter::ServiceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));
    //! [Service Discoverability]

    //! [Protocol descriptor list]
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
             << QVariant::fromValue(quint8(server->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);
    //! [Protocol descriptor list]

    // Register the service
    serviceInfo.registerService();

    // Save old Bluetooth hostmode - we restore it as soon this class is deleted.
    previousHostMode = localBluetoothDevice->hostMode();

    // Set new host mode
    localBluetoothDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverableLimitedInquiry);
}

void BluetoothMultiplayerAdapter::onNewRemoteConnection() {
    while (server->hasPendingConnections()) {
        QBluetoothSocket *socket = server->nextPendingConnection();

        qDebug() << "New remote connection from " << socket->peerAddress().toString();

        handleNewRemoteConnection(socket);

        connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(onRemoteSocketError(QBluetoothSocket::SocketError)));
    }
}

void BluetoothMultiplayerAdapter::onGameChanged() {
    if (game() == NULL) {
        if (server) {
            serviceInfo.unregisterService();

            server->close();
            server->deleteLater();
            server = NULL;
        }

        localBluetoothDevice->setHostMode(previousHostMode);

        return;
    }

    startServer();
}

BluetoothGameInfoModel::BluetoothGameInfoModel(QBluetoothServiceDiscoveryAgent *agent, QObject *parent) :
    GameInfoModel(parent), m_agent(agent) {

    if (agent->isActive())
        agent->stop();

    agent->start();

    connect(agent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), SLOT(onServiceDiscovered(QBluetoothServiceInfo)));
    connect(agent, SIGNAL(finished()), SLOT(onFinished()));
}

void BluetoothGameInfoModel::onServiceDiscovered(const QBluetoothServiceInfo &info) {
    BluetoothGameInfo *gameInfo = new BluetoothGameInfo(this);
    gameInfo->info = info;

    appendGameInfo(gameInfo);
}

void BluetoothGameInfoModel::onFinished() {
    m_state = Complete;

    emit stateChanged();
}
