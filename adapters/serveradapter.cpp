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

#include "serveradapter.h"
#include "abstractserver.h"
#include "../message.h"
#include "../board.h"
#include "../game.h"
#include "../player.h"

#include <QIODevice>

class PlayerInfo {
public:
    enum State {
        Connecting = 1,
        Connected = 2,
        Disconnected = 4
    };
public:
    PlayerInfo(ClientInfo *clientInfo);
public:
    ClientInfo *clientInfo;
    QIODevice *device;
    QByteArray buffer;
    State state;
    Player *player;
};

ClientInfo::ClientInfo(QIODevice *device) :
    QObject(device), m_device(device) {

}

QIODevice *ClientInfo::device() const {
    return m_device;
}

PlayerInfo::PlayerInfo(ClientInfo *clientInfo) :
    clientInfo(clientInfo), device(clientInfo->device()), state(Connecting),
    player(NULL) {

}

ServerAdapter::ServerAdapter(QObject *parent) :
    QObject(parent), m_game(NULL), m_board(NULL)
{

}

/**
  * Sets the current game on this adapter. The adapter will retrieve its
  * values from the given game and make changes to this game when new
  * messages arrive.
  */
void ServerAdapter::setGame(Game *game) {
    if (game == m_game)
        return;

    if (m_game) {
        disconnect(m_game, SIGNAL(boardChanged()),
                   this, SLOT(onBoardChanged()));
        onBoardChanged();
    }

    m_game = game;

    if (!m_game) {
        foreach(QIODevice *device, m_players.keys())
            removeClient(device);

        foreach (AbstractServer *server, m_attachedServers)
            server->disable();

        if (m_board)
            disconnect(m_board, SIGNAL(cellValueChanged(Cell*)),
                       this, SLOT(onCellValueChanged(Cell*)));
        m_board = NULL;
    } else {
        connect(m_game, SIGNAL(boardChanged()), SLOT(onBoardChanged()));
        onBoardChanged();

        foreach (AbstractServer *server, m_attachedServers)
            server->enable();
    }
}

/**
  * Callback to connect to cell value changes of the current board.
  */
void ServerAdapter::onBoardChanged() {
    Board *board = m_game->board();

    if (board == m_board)
        return;

    if (m_board)
        disconnect(m_board, SIGNAL(cellValueChanged(Cell*)),
                   this, SLOT(onCellValueChanged(Cell*)));

    m_board = board;

    if (!m_board)
        return;

    connect(m_board,
            SIGNAL(cellValueChanged(Cell*)),
            SLOT(onCellValueChanged(Cell*)));
}

/**
  * Transmits changed cell values to all connected clients.
  */
void ServerAdapter::onCellValueChanged(Cell *cell) {
    QUuid uuid;

    if (cell->valueOwner() != NULL)
        uuid = cell->valueOwner()->uuid();

    SetValueMessage message(cell->x(), cell->y(), cell->value(), uuid);
    sendMessage(message);
}

/**
  * Sends the given message to all clients which are in the Connected state.
  *
  */
void ServerAdapter::sendMessage(Message &message) {
    foreach (const PlayerInfo *playerInfo, m_players.values()) {
        if (playerInfo->state != PlayerInfo::Connected)
            continue;

        sendMessage(*playerInfo, message);
    }
}

/**
  * Sends the given message to the specified player.
  */
void ServerAdapter::sendMessage(const PlayerInfo &playerInfo, Message &message) {
    QDataStream dataStream(playerInfo.device);

    if (!message.writeStream(dataStream)) {
        qWarning() << "Failed to transmit message to " << playerInfo.device;
    }
}

/**
  * Adds a server implementation to this adapter.
  */
void ServerAdapter::addServerImplementation(AbstractServer *serverImpl) {
    serverImpl->setServerAdapter(this);

    m_attachedServers.append(serverImpl);
}

/**
  * Returns all registered server implementations.
  */
const QList<AbstractServer *> &ServerAdapter::serverImplementations() const {
    return m_attachedServers;
}

/**
  * Adds a new client to this server adapter. This function will generate
  * a new PlayerInfo entry in the Connecting state for the given device
  * and handle all message processing.
  */
ClientInfo *ServerAdapter::addClient(QIODevice *device) {
    ClientInfo *clientInfo = new ClientInfo(device);
    PlayerInfo *playerInfo = new PlayerInfo(clientInfo);

    m_players[device] = playerInfo;

    connect(device, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(device,
            SIGNAL(readChannelFinished()),
            SLOT(onReadChannelFinished()));

    HelloMessage message;
    sendMessage(*playerInfo, message);

    return clientInfo;
}

/**
  * Removes the client bound to the given \a device from the player list.
  *
  * The \a device will be deleted in this function so you should not
  * access it afterwards.
  */
void ServerAdapter::removeClient(QIODevice *device) {
    PlayerInfo *playerInfo = m_players.take(device);
    Q_ASSERT(playerInfo);

    if (playerInfo->player)
        playerInfo->player->setState(Player::Disconnected);

    disconnect(device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    disconnect(device, SIGNAL(readChannelFinished()),
               this, SLOT(onReadChannelFinished()));

    device->close();
    device->deleteLater();

    emit playerInfo->clientInfo->removed();

    delete playerInfo;

    qDebug() << "Removed client " << device;
}

/**
  * Returns whether a client bound to the given QIODevice exists.
  */
bool ServerAdapter::hasClient(QIODevice *device) const {
    return m_players.contains(device);
}

/**
  * Reads data sent from a previously added device handling the messages sent
  * appropriately.
  */
void ServerAdapter::onReadyRead() {
    QIODevice *device = qobject_cast<QIODevice *>(sender());
    Q_ASSERT(device);

    PlayerInfo *playerInfo = m_players[device];
    Q_ASSERT(playerInfo);

    playerInfo->buffer.append(device->read(device->bytesAvailable()));

    parseMessages(*playerInfo);
}

/**
  * Removes player from connected list if there is no more data to receive.
  */
void ServerAdapter::onReadChannelFinished() {
    QIODevice *device = qobject_cast<QIODevice *>(sender());
    Q_ASSERT(device);

    removeClient(device);
}

/**
  * This function parses all messages within the buffer of the given
  * PlayerInfo instance.
  */
void ServerAdapter::parseMessages(PlayerInfo &playerInfo) {
    QDataStream dataStream(playerInfo.buffer);

    while (!dataStream.atEnd()) {
        Message *message = Message::parse(dataStream);

        if (message == NULL)
            break;

        qDebug() << "Received message with " << message->type();

        switch (message->type()) {
        case Message::JoinMessage:
            handleJoinMessage(playerInfo, (JoinMessage *) message);
            break;
        case Message::SetValueMessage:
            handleSetValueMessage(playerInfo, (SetValueMessage *) message);
            break;
        default:
            break;
        }

        delete message;
    }

    // Remove read messages
    playerInfo.buffer.remove(0, dataStream.device()->pos());
}

/**
  * Handles a join message sent by the given player.
  */
void ServerAdapter::handleJoinMessage(PlayerInfo &playerInfo,
                                      JoinMessage *message) {
    qDebug() << "Player with uuid" << message->uuid() << "has joined the game.";

    if (m_game == NULL) {
        removeClient(playerInfo.device);
    } else {
        GameMessage gameMessage(m_game);
        sendMessage(playerInfo, gameMessage);

        playerInfo.player = m_game->addPlayer(message->uuid(), message->name());
        playerInfo.player->setState(Player::Connected);
        playerInfo.state = PlayerInfo::Connected;
    }
}

/**
  * Handles a SetValue message sent from the given player.
  */
void ServerAdapter::handleSetValueMessage(PlayerInfo &playerInfo, SetValueMessage *message) {
    Q_UNUSED(playerInfo)

    if (m_game == NULL || m_game->board() == NULL)
        return;

    Cell *cell = m_game->board()->cellAt(message->x(), message->y());

    if (cell == NULL) {
        qDebug() << "Request for non-existent cell at X:" << message->x() << "Y:" << message->y();
        return;
    }

    if (cell->isFixedCell()) {
        qDebug() << "User attempted to modify fixed cell";
        return;
    }

    QList<QObject *> invalidCells = m_game->board()->isValidMove(message->x(),
                                                                 message->y(),
                                                                 message->value());
    if (!invalidCells.isEmpty()) {
        // The boards are not in an inconsistent state - as the server we leave
        // the current cell value as is and retransmit it to the clients.
        onCellValueChanged(cell);
        return;
    }

    foreach (Player *existingPlayer, (const QList<Player *> &) m_game->players()) {
        if (existingPlayer->uuid() != message->valueOwner())
            continue;

        cell->setValueOwner(existingPlayer);
        break;
    }

    cell->setValue(message->value());
}
