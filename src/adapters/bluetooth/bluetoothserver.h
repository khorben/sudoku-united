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

#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

#include "../abstractserver.h"

#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#ifdef HAVE_SYSTEM_DEVICE_INFO
#include <QSystemDeviceInfo>
#endif

QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE
class QRfcommServer;
class QBluetoothServiceInfo;
QTM_END_NAMESPACE

class BluetoothServer : public AbstractServer
{
    Q_OBJECT
public:
    explicit BluetoothServer(QObject *parent = 0);
    ~BluetoothServer();

    GameInfoModel *discoverGames();

    void enable();
    void disable();

private slots:
    void onNewConnection();
    void onSocketError(QBluetoothSocket::SocketError error);
    void onClientRemoved();
#ifdef HAVE_SYSTEM_DEVICE_INFO
    void onCurrentProfileChanged(QSystemDeviceInfo::Profile profile);
#endif
    void onHostModeStateChanged(QBluetoothLocalDevice::HostMode hostMode);
private:
    void registerService();
    void unregisterService();
private:
    QRfcommServer *server;
    QBluetoothServiceInfo *serviceInfo;

    QBluetoothLocalDevice *localBluetoothDevice;
    QBluetoothLocalDevice::HostMode previousHostMode;

#ifdef HAVE_SYSTEM_DEVICE_INFO
    QSystemDeviceInfo *systemDeviceInfo;
#endif
};

#endif // BLUETOOTHSERVER_H
