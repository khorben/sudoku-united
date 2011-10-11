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

#include "bluetoothserver.h"
#include "bluetoothgameinfo.h"
#include "../serveradapter.h"

#include <QRfcommServer>

BluetoothServer::BluetoothServer(QObject *parent) :
    AbstractServer(parent), server(NULL), serviceInfo(NULL),
    localBluetoothDevice(NULL)
{
    systemDeviceInfo = new QSystemDeviceInfo(this);
    connect(systemDeviceInfo,
            SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            SLOT(onCurrentProfileChanged(QSystemDeviceInfo::Profile)));

    localBluetoothDevice = new QBluetoothLocalDevice(this);
}

BluetoothServer::~BluetoothServer() {
    if (serviceInfo) {
        unregisterService();
        delete serviceInfo;
    }
}

GameInfoModel *BluetoothServer::discoverGames() {
    if (systemDeviceInfo->currentProfile() == QSystemDeviceInfo::OfflineProfile) {
        return NULL;
    }

    return new BluetoothGameInfoModel(this);
}

void BluetoothServer::enable() {
    if (!localBluetoothDevice->isValid())
        return;

    if (systemDeviceInfo->currentProfile() == QSystemDeviceInfo::OfflineProfile) {
        qDebug() << "Device in offline profile - not enabling Bluetooth";
        return;
    }

    qDebug() << "Enabling Bluetooth server";

    if (server) {
        disable();
    }

    server = new QRfcommServer(this);

    // There seems to be a problem if channel id 1 is used so we
    // force a port >= 2.
    quint16 port = 2;
    while (!server->listen(QBluetoothAddress(), port) && port < 24)
        port++;

    if (!server->isListening()) {
        qWarning() << "Failed to start Bluetooth server.";
        return;
    }

    connect(server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    registerService();
}



void BluetoothServer::disable() {
    unregisterService();

    if (!server)
        return;

    server->close();
    server->deleteLater();
    server = NULL;
}

void BluetoothServer::registerService() {
    if (!serviceInfo)
        serviceInfo = new QBluetoothServiceInfo();

    // Heavily based on Qt Mobility's Bluetooth chat sample
    serviceInfo->setAttribute(QBluetoothServiceInfo::ServiceRecordHandle,
                              (uint)0x00010010);

    //! [Class Uuuid must contain at least 1 entry]
    QBluetoothServiceInfo::Sequence classId;
    classId << QVariant::fromValue(QBluetoothUuid(BLUETOOTH_SERVICE_UUID));
    serviceInfo->setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);
    //! [Class Uuuid must contain at least 1 entry]

    //! [Service name, description and provider]
    serviceInfo->setAttribute(QBluetoothServiceInfo::ServiceName, tr("Sudoku United Game Server"));
    serviceInfo->setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("A multiplayer server for the Sudoku United game."));
    serviceInfo->setAttribute(QBluetoothServiceInfo::ServiceProvider, tr(""));
    //! [Service name, description and provider]

    //! [Service UUID set]
    serviceInfo->setServiceUuid(QBluetoothUuid(BLUETOOTH_SERVICE_UUID));
    //! [Service UUID set]

    //! [Service Discoverability]
    serviceInfo->setAttribute(QBluetoothServiceInfo::BrowseGroupList,
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
    serviceInfo->setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);
    //! [Protocol descriptor list]

    // Save old Bluetooth hostmode - we restore it as soon the server is
    // disabled.
    previousHostMode = localBluetoothDevice->hostMode();

    // Set new host mode if needed
    if (localBluetoothDevice->hostMode() != QBluetoothLocalDevice::HostDiscoverable
            && localBluetoothDevice->hostMode() != QBluetoothLocalDevice::HostDiscoverableLimitedInquiry) {
        connect(localBluetoothDevice,
                SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
                SLOT(onHostModeStateChanged(QBluetoothLocalDevice::HostMode)));
        localBluetoothDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverableLimitedInquiry);
    }
    // Register the service
    if (!serviceInfo->registerService())
        qWarning() << "Failed to register Bluetooth service";
}

void BluetoothServer::unregisterService() {
    if (!serviceInfo)
        return;

    serviceInfo->unregisterService();

    localBluetoothDevice->setHostMode(previousHostMode);
}

void BluetoothServer::onCurrentProfileChanged(QSystemDeviceInfo::Profile profile) {
    if (profile == QSystemDeviceInfo::OfflineProfile)
        disable();
    else if (serverAdapter()->inGame() && !server)
        enable();
}

void BluetoothServer::onNewConnection() {
    while (server->hasPendingConnections()) {
        QBluetoothSocket *socket = server->nextPendingConnection();

        qDebug() << "New remote connection from " << socket->peerAddress().toString();

        connect(socket,
                SIGNAL(error(QBluetoothSocket::SocketError)),
                SLOT(onSocketError(QBluetoothSocket::SocketError)));

        ClientInfo *clientInfo = serverAdapter()->addClient(socket);
        connect(clientInfo, SIGNAL(removed()), SLOT(onClientRemoved()));
    }
}

void BluetoothServer::onSocketError(QBluetoothSocket::SocketError error) {
    Q_UNUSED(error);

    QBluetoothSocket *socket = qobject_cast<QBluetoothSocket *>(sender());
    Q_ASSERT(socket);

    serverAdapter()->removeClient(socket);
}

/**
  * Handles the case when a client is removed from the adapter.
  * This method is used to unsubscribe all QBluetoothSocket
  * specific signals from this class.
  */
void BluetoothServer::onClientRemoved() {
    ClientInfo *clientInfo = qobject_cast<ClientInfo *>(sender());
    Q_ASSERT(clientInfo);

    QBluetoothSocket *socket =
            qobject_cast<QBluetoothSocket *>(clientInfo->device());
    Q_ASSERT(socket);

    disconnect(socket, 0, this, 0);
}

/**
  * Workaround for a problem on Harmattan where setting a powered off device to
  * Discoverable would only put it into the connectable state.
  */
void BluetoothServer::onHostModeStateChanged(QBluetoothLocalDevice::HostMode hostMode)
{
    disconnect(localBluetoothDevice,
               SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
               this,
               SLOT(onHostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    if (hostMode == QBluetoothLocalDevice::HostConnectable && server)
        localBluetoothDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverableLimitedInquiry);

}
