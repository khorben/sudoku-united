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

#include "bluetoothclient.h"
#include "bluetoothgameinfo.h"

#include <QBluetoothDeviceInfo>

BluetoothClient::BluetoothClient(QObject *parent) :
    AbstractClient(parent)
{
    systemDeviceInfo = new QSystemDeviceInfo(this);
    connect(systemDeviceInfo,
            SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            SLOT(onCurrentProfileChanged(QSystemDeviceInfo::Profile)));
}

void BluetoothClient::join(GameInfo *game) {
    BluetoothGameInfo *gameInfo = qobject_cast<BluetoothGameInfo *>(game);

    if (gameInfo == NULL)
        return;

    setState(Connecting);

    if (systemDeviceInfo->currentProfile()
            == QSystemDeviceInfo::OfflineProfile) {
        setError("Offline mode is active.");
        setState(Disconnected);
    }

    // Disable service discovery
    BluetoothGameInfoModel::agent()->stop();

    // Completely reset the Bluetooth socket as there have been segmentation
    // faults when reusing the old one.
    QBluetoothSocket *socket = (QBluetoothSocket *) device();

    if (socket) {
        socket->close();
        socket->deleteLater();
    }

    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);
    setDevice(socket);

    connect(socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(socket,
            SIGNAL(error(QBluetoothSocket::SocketError)),
            SLOT(onSocketError(QBluetoothSocket::SocketError)));

    qDebug() << "Connecting to service"
             << gameInfo->info.device().address().toString()
             << gameInfo->info.serverChannel();

    socket->connectToService(gameInfo->info);
}

void BluetoothClient::leave() {
    setState(Disconnected);

    QBluetoothSocket *socket = (QBluetoothSocket *) device();
    if (socket) {
        socket->deleteLater();
        setDevice(NULL);
    }
}

void BluetoothClient::onConnected() {
    startTimeoutTimer();
}

void BluetoothClient::onSocketError(QBluetoothSocket::SocketError error) {
    Q_UNUSED(error)

    setError(device()->errorString());
    leave();
}

void BluetoothClient::onCurrentProfileChanged(QSystemDeviceInfo::Profile profile) {
    if (profile == QSystemDeviceInfo::OfflineProfile) {
        setError("Offline profile activated.");
        setState(Disconnected);
    }
}
