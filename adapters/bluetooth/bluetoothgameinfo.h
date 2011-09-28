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

#ifndef BLUETOOTHGAMEINFO_H
#define BLUETOOTHGAMEINFO_H

#include "../gameinfo.h"

#include <QBluetoothServiceInfo>
#include <QBluetoothServiceDiscoveryAgent>

QTM_USE_NAMESPACE

#define BLUETOOTH_SERVICE_UUID QLatin1String("614b8c48-c0af-4d20-b4d7-a24dfcd9900e")

class BluetoothGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    BluetoothGameInfoModel(QObject *parent);

    static QBluetoothServiceDiscoveryAgent *agent();
private slots:
    void onServiceDiscovered(const QBluetoothServiceInfo &info);
    void onFinished();
    void onAutoRefreshChanged();
    void onError(QBluetoothServiceDiscoveryAgent::Error error);
    void startDiscovery();
private:
    QList<GameInfo *> newGameInfoEntries;
    QTimer *autoRefreshTimer;
    QMutex mutex;
private:
    static QBluetoothServiceDiscoveryAgent *m_agent;
};

class BluetoothGameInfo : public GameInfo {
    Q_OBJECT
public:
    BluetoothGameInfo(QObject *parent = 0);

    QBluetoothServiceInfo info;

    QString name() const;
    AbstractClient *client() const;

    bool operator ==(const GameInfo &other) const;
};


#endif // BLUETOOTHGAMEINFO_H
