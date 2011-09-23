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

#ifndef BLUETOOTHMULTIPLAYERADAPTER_H
#define BLUETOOTHMULTIPLAYERADAPTER_H

#include "multiplayeradapter.h"


#include <QBluetoothUuid>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QBluetoothServiceInfo>


QTM_BEGIN_NAMESPACE
class QRfcommServer;
class QBluetoothServiceDiscoveryAgent;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class BluetoothMultiplayerAdapter : public MultiplayerAdapter
{
    Q_OBJECT
public:
    /**
      * The UUID used to register the Sudoku United service.
      */
    static const QLatin1String ServiceUuid;
public:
    static bool hostSupportsBluetooth();
public:
    explicit BluetoothMultiplayerAdapter(Sudoku *parent);
    ~BluetoothMultiplayerAdapter();

    GameInfoModel *discoverGames();

    void join(GameInfo *game);

    bool canJoinGameInfo(GameInfo *game) const;
signals:

public slots:

private slots:
    void onLocalSocketError(QBluetoothSocket::SocketError error);
    void onRemoteSocketError(QBluetoothSocket::SocketError error);
    void onNewRemoteConnection();
    void onGameChanged();
    void startServer();
private:
    void setupService();
    void setupLocalSocket();
private:
    QBluetoothServiceInfo serviceInfo;
    QBluetoothServiceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localBluetoothDevice;
    QRfcommServer *server;

    QBluetoothLocalDevice::HostMode previousHostMode;
};

class BluetoothGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    BluetoothGameInfoModel(QBluetoothServiceDiscoveryAgent *agent, QObject *parent);
private slots:
    void onServiceDiscovered(const QBluetoothServiceInfo &info);
    void onFinished();
private:
    QBluetoothServiceDiscoveryAgent *m_agent;
};

class BluetoothGameInfo : public GameInfo {
    Q_OBJECT
public:
    BluetoothGameInfo(QObject *parent = 0);

    QBluetoothServiceInfo info;

    QString name() const;
    quint16 playerCount() const;
};


#endif // BLUETOOTHMULTIPLAYERADAPTER_H
