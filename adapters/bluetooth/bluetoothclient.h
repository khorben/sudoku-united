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

#ifndef BLUETOOTHCLIENT_H
#define BLUETOOTHCLIENT_H

#include "../abstractclient.h"

#include <QBluetoothSocket>
#include <QSystemDeviceInfo>

QTM_USE_NAMESPACE
class BluetoothClient : public AbstractClient
{
    Q_OBJECT
public:
    explicit BluetoothClient(QObject *parent = 0);

    void join(GameInfo *gameInfo);
    void leave();
signals:

private slots:
    void onConnected();
    void onSocketError(QBluetoothSocket::SocketError error);
    void onCurrentProfileChanged(QSystemDeviceInfo::Profile profile);
private:
    QSystemDeviceInfo *systemDeviceInfo;

};

#endif // BLUETOOTHCLIENT_H
