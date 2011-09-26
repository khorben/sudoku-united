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

#include "bluetoothgameinfo.h"
#include "bluetoothclient.h"

#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothDeviceInfo>

BluetoothGameInfo::BluetoothGameInfo(QObject *parent) :
    GameInfo(parent)
{
}

QString BluetoothGameInfo::name() const {
     return info.device().name();
}

AbstractClient *BluetoothGameInfo::client() const {
    return new BluetoothClient();
}

bool BluetoothGameInfo::operator ==(const GameInfo &other) const {
    const BluetoothGameInfo *otherGameInfo =
            qobject_cast<const BluetoothGameInfo *>(&other);

    if (!otherGameInfo)
        return false;

    return info.device().address() == otherGameInfo->info.device().address() &&
            info.serverChannel() == otherGameInfo->info.serverChannel();
}


BluetoothGameInfoModel::BluetoothGameInfoModel(QObject *parent) :
    GameInfoModel(parent) {

    m_agent = new QBluetoothServiceDiscoveryAgent(this);
    m_agent->setUuidFilter(QBluetoothUuid(BLUETOOTH_SERVICE_UUID));
    m_agent->start();

    connect(m_agent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), SLOT(onServiceDiscovered(QBluetoothServiceInfo)));
    connect(m_agent, SIGNAL(finished()), SLOT(onFinished()));
}

void BluetoothGameInfoModel::onServiceDiscovered(const QBluetoothServiceInfo &info) {
    BluetoothGameInfo *gameInfo = new BluetoothGameInfo(this);
    gameInfo->info = info;

    insertGameInfo(gameInfo);
}

void BluetoothGameInfoModel::onFinished() {
    m_state = Complete;

    emit stateChanged();
}
