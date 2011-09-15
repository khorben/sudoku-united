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

#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceInfo>
#include <QRfcommServer>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothLocalDevice>

const QBluetoothUuid BluetoothMultiplayerAdapter::ServiceUuid(QLatin1String("614b8c48-c0af-4d20-b4d7-a24dfcd9900e"));

BluetoothGameInfo::BluetoothGameInfo(QObject *parent) :
    GameInfo(parent) {

}

QString BluetoothGameInfo::name() const {
     return info.device().name();
}

quint16 BluetoothGameInfo::playerCount() const {
    return info.attribute(BluetoothMultiplayerAdapter::PlayerCountServiceAttributeId).toUInt();
}

BluetoothMultiplayerAdapter::BluetoothMultiplayerAdapter(Sudoku *parent) :
    MultiplayerAdapter(parent), server(NULL)
{
    server = new QRfcommServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    localDevice = new QBluetoothLocalDevice(this);

    discoveryAgent = new QBluetoothServiceDiscoveryAgent(this);
    discoveryAgent->setUuidFilter(BluetoothMultiplayerAdapter::ServiceUuid);

    m_local = new PlayerInfo();
    m_local->device = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);

    // Listen for game changes as we need to subscribe to the players changed property
    connect(this, SIGNAL(gameChanged()), SLOT(onGameChanged()));
}

BluetoothMultiplayerAdapter::~BluetoothMultiplayerAdapter() {
    localDevice->setHostMode(previousHostMode);
}

GameInfoModel *BluetoothMultiplayerAdapter::discoverGames() {
    return new BluetoothGameInfoModel(discoveryAgent, this);
}

void BluetoothMultiplayerAdapter::join(GameInfo *game) {
    BluetoothGameInfo *gameInfo = qobject_cast<BluetoothGameInfo *>(game);

    if (gameInfo == NULL)
        return;

    if (discoveryAgent->isActive())
        discoveryAgent->stop();

    qDebug() << "Connecting to service" << gameInfo->info.device().address().toString() << gameInfo->info.serverChannel();

    if (m_local->device->isOpen())
        m_local->device->close();

    QBluetoothSocket *socket = (QBluetoothSocket *) m_local->device;

    socket->connectToService(gameInfo->info);

    connect(socket, SIGNAL(connected()), SLOT(onConnectedToServer()));
    connect(socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(socket, SIGNAL(readChannelFinished()), SLOT(onReadChannelFinished()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), SLOT(onSocketError(QBluetoothSocket::SocketError)));
}

void BluetoothMultiplayerAdapter::onSocketError(QBluetoothSocket::SocketError error) {
    qDebug() << "Bluetooth socket error:" << error;
}

void BluetoothMultiplayerAdapter::startServer() {
    if (server->isListening())
        return;

    if (!server->listen()) {
        qDebug() << "Failed to listen on Bluetooth server socket.";

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

    // Add player count
    quint16 playerCount = 0;
    if (game() != NULL)
        playerCount = game()->players().size();

    serviceInfo.setAttribute(BluetoothMultiplayerAdapter::PlayerCountServiceAttributeId, playerCount);

    // Register the service
    serviceInfo.registerService();


    // Save old Bluetooth hostmode - we restore it as soon this class is deleted.
    previousHostMode = localDevice->hostMode();

    // Set new host mode
    localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverableLimitedInquiry);
}

void BluetoothMultiplayerAdapter::onNewConnection() {
    while (server->hasPendingConnections()) {
        QBluetoothSocket *socket = server->nextPendingConnection();

        handleNewConnection(socket);

        connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(onSocketError(QBluetoothSocket::SocketError)));
    }
}

void BluetoothMultiplayerAdapter::onGameChanged() {
    if (game() == NULL) {
        if (server)
            server->close();

        localDevice->setHostMode(previousHostMode);

        return;
    }

    startServer();

    connect(game(), SIGNAL(playersChanged()), SLOT(onPlayersChanged()));
}

void BluetoothMultiplayerAdapter::onPlayersChanged() {
    serviceInfo.unregisterService();

    serviceInfo.setAttribute(BluetoothMultiplayerAdapter::PlayerCountServiceAttributeId, quint16(game()->players().size()));

    serviceInfo.registerService();
}

BluetoothGameInfoModel::BluetoothGameInfoModel(QBluetoothServiceDiscoveryAgent *agent, QObject *parent) :
    GameInfoModel(parent), m_agent(agent) {

    if (agent->isActive())
        agent->stop();

    agent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

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
