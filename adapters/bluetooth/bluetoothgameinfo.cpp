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

QBluetoothServiceDiscoveryAgent *BluetoothGameInfoModel::m_agent = NULL;

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

    return info.device().address().toUInt64() == otherGameInfo->info.device().address().toUInt64() &&
            info.serverChannel() == otherGameInfo->info.serverChannel();
}


BluetoothGameInfoModel::BluetoothGameInfoModel(QObject *parent) :
    GameInfoModel(parent), localDevice(NULL) {

    localDevice = new QBluetoothLocalDevice(this);
    if (!localDevice->isValid()) {
        // There does not seem to be a Bluetooth adapter in this device. No
        // point in continuing.
        setState(Complete);
        return;
    }

    previousHostMode = localDevice->hostMode();

    autoRefreshTimer = new QTimer(this);
    autoRefreshTimer->setSingleShot(true);
    connect(autoRefreshTimer, SIGNAL(timeout()), SLOT(startDiscovery()));

    QBluetoothServiceDiscoveryAgent *agent = BluetoothGameInfoModel::agent();

    connect(agent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            SLOT(onServiceDiscovered(QBluetoothServiceInfo)));
    connect(agent, SIGNAL(finished()), SLOT(onFinished()));
    connect(agent, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)),
            SLOT(onError(QBluetoothServiceDiscoveryAgent::Error)));
    connect(this, SIGNAL(autoRefreshChanged()), SLOT(onAutoRefreshChanged()));

    startDiscovery();
}

BluetoothGameInfoModel::~BluetoothGameInfoModel() {
    if (localDevice && localDevice->isValid()) {
        localDevice->setHostMode(previousHostMode);
    }
}

QBluetoothServiceDiscoveryAgent *BluetoothGameInfoModel::agent() {
    if (!m_agent) {
        m_agent = new QBluetoothServiceDiscoveryAgent();
        m_agent->setUuidFilter(QBluetoothUuid(BLUETOOTH_SERVICE_UUID));
    }

    return m_agent;
}

void BluetoothGameInfoModel::onServiceDiscovered(const QBluetoothServiceInfo &info) {
    BluetoothGameInfo *gameInfo = new BluetoothGameInfo(this);
    gameInfo->info = info;

    insertGameInfo(gameInfo);
    newGameInfoEntries.append(gameInfo);
}

void BluetoothGameInfoModel::onFinished() {
    // Clear stale entries
    int row = -1;
    foreach (GameInfo *gameInfo, m_gameInfoList) {
        row++;

        if (!qobject_cast<BluetoothGameInfo *>(gameInfo))
            continue;

        bool foundMatch = false;
        foreach (GameInfo *newGameInfo, newGameInfoEntries) {
            if (*newGameInfo == *gameInfo) {
                foundMatch = true;
                break;
            }
        }

        if (foundMatch)
            continue;

        // Stale entry remove it
        beginRemoveRows(QModelIndex(), row, row);
        m_gameInfoList.removeAt(row);
        endRemoveRows();
        row--;
    }

    setState(Complete);

    if (autoRefresh())
        autoRefreshTimer->start(5000);
}

void BluetoothGameInfoModel::onError(QBluetoothServiceDiscoveryAgent::Error error) {
    qWarning() << "Service discovery failed: " << error;

    setState(Complete);

    if (autoRefresh())
        autoRefreshTimer->start(5000);
}

void BluetoothGameInfoModel::onAutoRefreshChanged() {
    if (autoRefresh() && !BluetoothGameInfoModel::agent()->isActive())
        autoRefreshTimer->start(5000);
    else if (!autoRefresh())
        autoRefreshTimer->stop();
}

void BluetoothGameInfoModel::startDiscovery() {
    if (localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        // We need to turn on the adapter if it is turned off
        localDevice->setHostMode(QBluetoothLocalDevice::HostConnectable);
    }

    QBluetoothServiceDiscoveryAgent *agent = BluetoothGameInfoModel::agent();

    setState(Discovering);

    if (!agent->isActive()) {
        newGameInfoEntries.clear();
        agent->clear();

        // Clearing also seems to reset the UUID filter - set it again
        agent->setUuidFilter(QBluetoothUuid(BLUETOOTH_SERVICE_UUID));
        agent->start();
    }

    foreach (QBluetoothServiceInfo serviceInfo, agent->discoveredServices()) {
        onServiceDiscovered(serviceInfo);
    }
}
