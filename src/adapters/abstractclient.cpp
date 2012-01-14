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

#include "abstractclient.h"
#include "../messages/message.h"
#include "../messages/setvaluemessage.h"
#include "../messages/hellomessage.h"
#include "../messages/joinmessage.h"
#include "../messages/gamemessage.h"
#include "../messages/playermessage.h"
#include "../game.h"
#include "../sudoku.h"
#include "../player.h"

#include <QIODevice>

AbstractClient::AbstractClient(QObject *parent) :
    QObject(parent), m_state(Disconnected), m_device(NULL), m_game(NULL),
    m_board(NULL)
{
    connect(&timeoutTimer, SIGNAL(timeout()), SLOT(onTimeout()));
}

AbstractClient::State AbstractClient::state() const {
    return m_state;
}
/**
  * Sets the connection state of this client adapter. Use the \a error
  * property to determine the error message if an unexpected state changed
  * happened.
  */
void AbstractClient::setState(State state) {
    if (m_state == state)
        return;

    m_state = state;

    if (m_state == Disconnected && m_device) {
        m_device->close();
    }

    switch (m_state) {
    case Connected:
    case Disconnected:
        stopTimeoutTimer();
        break;
    case Connecting:
        break;
    }

    emit stateChanged(m_state);
}

/**
  * Sets the error message associated with this adapter.
  */
void AbstractClient::setError(const QString &error) {
    m_error = error;
}

/**
  * Returns the error message associated with this client adapter.
  */
QString AbstractClient::error() const {
    return m_error;
}

/**
  * Returns the device associated with this client adapter. The device is used
  * to receive and transmit messages.
  */
QIODevice *AbstractClient::device() const {
    return m_device;
}

/**
  * Sets the device associated with this client adapter. The device is used
  * to receive and transmit messages.
  */
void AbstractClient::setDevice(QIODevice *device) {
    if (m_device == device)
        return;

    if (m_device) {
        disconnect(m_device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        disconnect(m_device, SIGNAL(readChannelFinished()),
                   this, SLOT(onReadChannelFinished()));
    }

    m_device = device;

    if (m_device) {
        connect(m_device, SIGNAL(readyRead()), SLOT(onReadyRead()));
        connect(m_device,
                SIGNAL(readChannelFinished()),
                SLOT(onReadChannelFinished()));
    }
}

/**
  * Returns the game this client adapter is operating on. This value is only
  * valid if the adapter is in the Connected state.
  */
Game *AbstractClient::game() const {
    return m_game;
}


/**
  * Callback to parse messages sent from server.
  */
void AbstractClient::onReadyRead() {
    Q_ASSERT(m_device);

    buffer.append(m_device->read(m_device->bytesAvailable()));

    parseMessages();
}

/**
  * Callback to disconnect once the connection is closed.
  */
void AbstractClient::onReadChannelFinished() {
    Q_ASSERT(m_device);

    // Clear read buffer
    buffer.clear();

    setState(Disconnected);
}

/**
  * This function parses all messages within the local message buffer.
  */
void AbstractClient::parseMessages() {
    QDataStream dataStream(buffer);

    while (!dataStream.atEnd()) {
        Message *message = Message::parse(dataStream);

        if (message == NULL)
            break;

        qDebug() << "Received message with " << message->type();

        switch (message->type()) {
        case Message::SetValueMessage:
            handleSetValueMessage((SetValueMessage *) message);
            break;
        case Message::HelloMessage:
            handleHelloMessage((HelloMessage *) message);
            break;
        case Message::GameMessage:
            handleGameMessage((GameMessage *) message);
            break;
        case Message::PlayerMessage:
            handlePlayerMessage((PlayerMessage *) message);
            break;
        default:
            break;
        }

        delete message;
    }

    // Remove read messages
    buffer.remove(0, dataStream.device()->pos());
}

/**
  * Handles a SetValue message.
  */
void AbstractClient::handleSetValueMessage(SetValueMessage *message) {
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

    QList<QObject *> invalidCells = game()->board()->isValidMove(message->x(),
                                                                 message->y(),
                                                                 message->value());
    if (!invalidCells.isEmpty()) {
        // The boards are not in an inconsistent state - we always accept the
        // servers value and clear the conflicting cells
        foreach(QObject *invalidCell, invalidCells) {
            ((Cell *) invalidCell)->setValue(0);
        }
    }


    foreach (Player *existingPlayer, (const QList<Player *> &) m_game->players()) {
        if (existingPlayer->uuid() != message->valueOwner())
            continue;

        cell->setValueOwner(existingPlayer);
        break;
    }

    cell->setValue(message->value());
}

/**
  * Handle a Hello message sent by the server.
  */
void AbstractClient::handleHelloMessage(HelloMessage *message) {
    if (message->protocolVersion() != PROTOCOL_VERSION) {
        setError("Protocol version mismatch.");
        setState(Disconnected);

        return;
    }

    // Reply with join message
    JoinMessage joinMessage(Sudoku::instance()->player()->uuid(), Sudoku::instance()->player()->name());
    sendMessage(joinMessage);
}

/**
  * Handle a game message sent by the server.
  */
void AbstractClient::handleGameMessage(GameMessage *message) {
    if (message->game() == NULL) {
        setState(Disconnected);
        return;
    }

    m_game = message->game();
    m_game->setParent(this);

    connect(m_game, SIGNAL(playerJoined(Player*)),
            this, SLOT(onPlayerChanged(Player*)));
    connect(m_game, SIGNAL(playerLeft(Player*)),
            this, SLOT(onPlayerChanged(Player*)));
    connect(m_game, SIGNAL(boardChanged()), SLOT(onBoardChanged()));
    onBoardChanged();

    // We are also part of the game
    m_game->addPlayer(Sudoku::instance()->player());

    setState(Connected);
}

void AbstractClient::handlePlayerMessage(PlayerMessage *message) {
    if (!m_game)
        return;

    Player *player =
            m_game->addPlayer(new Player(message->uuid(), message->name()));
    if (!player)
        return;
    player->setState(message->state());
}

/**
  * Sends the specified message to the server.
  */
void AbstractClient::sendMessage(Message &message) {
    if (m_state == Disconnected)
        return;

    QDataStream dataStream(m_device);

    if (!message.writeStream(dataStream))
        qWarning() << "Failed to send message to server: "
                   << m_device->errorString();
}

void AbstractClient::onBoardChanged() {
    if (m_board) {
        disconnect(m_board, SIGNAL(cellValueChanged(Cell*)),
                   this, SLOT(onCellValueChanged(Cell*)));
    }

    m_board = m_game->board();

    if (m_board) {
        connect(m_board,
                SIGNAL(cellValueChanged(Cell*)),
                SLOT(onCellValueChanged(Cell*)));
    }
}

void AbstractClient::onCellValueChanged(Cell *cell) {
    QUuid uuid;
    if (cell->valueOwner())
        uuid = cell->valueOwner()->uuid();

    SetValueMessage message(cell->x(), cell->y(), cell->value(), uuid);
    sendMessage(message);
}

void AbstractClient::onPlayerChanged(Player *player) {
    PlayerMessage message(player->uuid(), player->name(), player->state());
    sendMessage(message);
}


/**
  * Starts or restarts the timeout timer. This timer can be used to cancel a
  * join after a specific time interval if the connection has not been setup
  * by then.
  *
  * If the state changes to Connected or Disconnected this timer is stopped
  * automatically. You can always manually cancel the timeout via the
  * stopTimeoutTimer method.
  *
  * The join is cancelled by calling the leave() method.
  */
void AbstractClient::startTimeoutTimer(quint32 timeoutInMilliseconds) {
    if (timeoutTimer.isActive())
        timeoutTimer.stop();

    timeoutTimer.start(timeoutInMilliseconds);
}

void AbstractClient::stopTimeoutTimer() {
    timeoutTimer.stop();
}

void AbstractClient::onTimeout() {
    if (m_state == Connecting) {
        setError("Disconnecting due to timeout.");
        leave();
    }
}
