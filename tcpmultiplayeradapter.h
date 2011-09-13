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

#ifndef TCPMULTIPLAYERADAPTER_H
#define TCPMULTIPLAYERADAPTER_H

#include "multiplayeradapter.h"

#include <QHash>
#include <QHostAddress>

class QTcpServer;
class QTcpSocket;

class JoinMessage;
class SetValueMessage;
class GameMessage;

class ValueTuple;

class TCPPlayerInfo {
public:
    enum State {
        Connecting,
        Connected
    };

    TCPPlayerInfo();

    State state;
    QByteArray readBuffer;

};

class TCPGameInfo : public GameInfo {
    Q_OBJECT
public:
    TCPGameInfo(QObject *parent = 0);

    QHostAddress address;
    quint16 port;

    QString name() const { return "TCP Server"; }
    quint16 playerCount() const { return 0; }
};

class TCPMultiplayerAdapter : public MultiplayerAdapter
{
    Q_OBJECT
public:
    explicit TCPMultiplayerAdapter(QObject *parent = 0);

    void join(GameInfo *game);

    GameInfoModel *discoverGames();
private slots:
    void onNewConnection();
    void onGameChanged();
private:
private:
    QTcpServer *m_server;

};

class TCPGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    TCPGameInfoModel(QObject *parent);

protected slots:
    void addFakeServer();
};

#endif // TCPMULTIPLAYERADAPTER_H
