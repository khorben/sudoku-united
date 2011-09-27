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

#ifndef SERVERADAPTER_H
#define SERVERADAPTER_H

#include <QObject>
#include <QMap>

class QIODevice;
class PlayerInfo;
class AbstractServer;

class Player;
class Game;
class Board;
class Cell;

class Message;
class SetValueMessage;
class JoinMessage;
class PlayerMessage;

class ClientInfo : public QObject {
    Q_OBJECT
public:
    ClientInfo(QIODevice *device);

    QIODevice *device() const;
signals:
    /**
      * This signal is emitted after the client has been removed
      * from the server adapter.
      */
    void removed();

private:
    friend class ServerAdapter;

    QIODevice *m_device;
};

class PlayerInfo;

class ServerAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ServerAdapter(QObject *parent = 0);

    ClientInfo *addClient(QIODevice *device);
    void removeClient(QIODevice *device);
    bool hasClient(QIODevice *device) const;

    void addServerImplementation(AbstractServer *serverImpl);
    const QList<AbstractServer *> &serverImplementations() const;

    void setGame(Game *game);
signals:

public slots:

private slots:
    void onReadyRead();
    void onReadChannelFinished();

    void onBoardChanged();

    void onCellValueChanged(Cell *cell);

    void onPlayerChanged(Player *player);
private:
    void sendMessage(Message &message);
    void sendMessage(const PlayerInfo &playerInfo, Message &message);

    void parseMessages(PlayerInfo &playerInfo);
    void handleJoinMessage(PlayerInfo &playerInfo, JoinMessage *message);
    void handleSetValueMessage(PlayerInfo &playerInfo, SetValueMessage *message);
    void handlePlayerMessage(PlayerInfo &playerInfo, PlayerMessage *message);
private:
    Game *m_game;
    Board *m_board;
    QMap<QIODevice *, PlayerInfo *> m_players;
    QList<AbstractServer *>  m_attachedServers;
};

#endif // SERVERADAPTER_H
