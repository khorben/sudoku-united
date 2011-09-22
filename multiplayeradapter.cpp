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

#include "multiplayeradapter.h"
#include "game.h"
#include "message.h"
#include "player.h"
#include "board.h"
#include "sudoku.h"

PlayerInfo::PlayerInfo() :
    device(NULL), state(Disconnected), player(NULL) {

}

GameInfo::GameInfo(QObject *parent) :
    QObject(parent) {

}

MultiplayerAdapter::MultiplayerAdapter(Sudoku *parent) :
    QObject(parent), m_sudoku(parent), m_local(new PlayerInfo())
{
    connect(parent, SIGNAL(gameChanged()), SLOT(onGameChanged()));
}

void MultiplayerAdapter::join(GameInfo *game) {
    Q_UNUSED(game)

    disconnectLocalDevice();

    m_local->state = PlayerInfo::Connecting;
}

void MultiplayerAdapter::onGameChanged() {
    Game *game = m_sudoku->game();

    if (game != NULL) {
        connect(game, SIGNAL(boardChanged()), SLOT(onBoardChanged()));
        onBoardChanged();
    } else {
        disconnectLocalDevice();

        foreach (QIODevice *device, m_remote.keys()) {
            delete m_remote[device];
            device->deleteLater();
        }

        m_remote.clear();
    }
}

void MultiplayerAdapter::onCellValueChanged(Cell *cell) {
    QUuid uuid;
    if (cell->valueOwner() != NULL)
        uuid = cell->valueOwner()->uuid();

    SetValueMessage message(cell->x(), cell->y(), cell->value(), uuid);
    sendMessage(NULL, &message);
}

void MultiplayerAdapter::onBoardChanged() {
    Board *board = m_sudoku->game()->board();

    if (board == NULL)
        return;

    connect(board, SIGNAL(cellValueChanged(Cell*)), SLOT(onCellValueChanged(Cell*)));
}

void MultiplayerAdapter::handleNewRemoteConnection(QIODevice *device) {
    PlayerInfo *playerInfo = new PlayerInfo();

    playerInfo->device = device;
    playerInfo->state = PlayerInfo::Connecting;

    m_remote.insert(device, playerInfo);

    connect(device, SIGNAL(readyRead()), SLOT(onRemoteReadyRead()));
    connect(device, SIGNAL(readChannelFinished()), SLOT(onRemoteReadChannelFinished()));

    HelloMessage message;
    sendMessage(playerInfo, &message);
}

void MultiplayerAdapter::handleJoinMessage(PlayerInfo &playerInfo, JoinMessage *message) {
    qDebug() << "Player with uuid" << message->uuid() << "has joined the game.";

    if (game() == NULL) {
        playerInfo.device->deleteLater();
    } else {
        GameMessage gameMessage(game());
        sendMessage(&playerInfo, &gameMessage);

        playerInfo.player = game()->addPlayer(message->uuid(), message->name());
    }
}

void MultiplayerAdapter::handleGameMessage(PlayerInfo &playerInfo, GameMessage *message) {
    if (playerInfo.device != m_local->device) {
        qWarning() << "Received game message from non-server.";
        return;
    }

    playerInfo.state = PlayerInfo::Connected;

    if (message->game() == NULL)
        return;

    Game *game = message->game();
    // We are also part of the game
    game->addPlayer(Sudoku::instance()->player());

    emit joinSucceeded(message->game());
}

void MultiplayerAdapter::handleSetValueMessage(PlayerInfo &playerInfo, SetValueMessage *message) {
    Q_UNUSED(playerInfo)

    Game *currentGame = game();

    if (currentGame == NULL || currentGame->board() == NULL)
        return;

    Cell *cell = currentGame->board()->cellAt(message->x(), message->y());

    if (cell == NULL) {
        qDebug() << "Request for non-existent cell at X:" << message->x() << "Y:" << message->y();
        return;
    }

    if (cell->isFixedCell()) {
        qDebug() << "User attempted to modify fixed cell";
        return;
    }

    foreach (Player *existingPlayer, (const QList<Player *> &) game()->players()) {
        qDebug() << existingPlayer->uuid() << message->valueOwner();
        if (existingPlayer->uuid() != message->valueOwner())
            continue;

        cell->setValueOwner(existingPlayer);
        break;
    }

    cell->setValue(message->value());
}

void MultiplayerAdapter::handleHelloMessage(PlayerInfo &playerInfo, HelloMessage *message) {
    if (playerInfo.device != m_local->device) {
        qWarning() << "Received Hello message from non-server.";
        return;
    }

    if (message->protocolVersion() != MultiplayerAdapter::ProtocolVersion) {
        emit joinFailed("Unsupported protocol version.");
        m_local->device->close();

        return;
    }

    // Reply with join message
    JoinMessage joinMessage(Sudoku::instance()->player()->uuid(), Sudoku::instance()->player()->name());
    sendMessage(m_local, &joinMessage);
}

void MultiplayerAdapter::onConnectedToServer() {
    qDebug() << "Successfully established connection to server.";
}

Game *MultiplayerAdapter::game() const {
    return m_sudoku->game();
}

void MultiplayerAdapter::onLocalReadyRead() {
    Q_ASSERT(m_local->device);
    Q_ASSERT(m_local->device->isOpen());

    handleIncomingData(*m_local);
}

void MultiplayerAdapter::onRemoteReadyRead() {
    QIODevice *device = qobject_cast<QIODevice *>(sender());
    Q_ASSERT(device);

    PlayerInfo *playerInfo = m_remote[device];
    Q_ASSERT(playerInfo);

    handleIncomingData(*playerInfo);
}

void MultiplayerAdapter::handleIncomingData(PlayerInfo &playerInfo) {
    QIODevice *device = playerInfo.device;
    QByteArray buffer = device->read(device->bytesAvailable());

    playerInfo.buffer.append(buffer);

    parseMessages(playerInfo);
}

void MultiplayerAdapter::parseMessages(PlayerInfo &playerInfo) {
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
        case Message::GameMessage:
            handleGameMessage(playerInfo, (GameMessage *) message);
            break;
        case Message::HelloMessage:
            handleHelloMessage(playerInfo, (HelloMessage *) message);
            break;
        default:
            break;
        }

        delete message;
    }

    // Remove read messages
    playerInfo.buffer.remove(0, dataStream.device()->pos());
}

void MultiplayerAdapter::onLocalReadChannelFinished() {
    disconnectLocalDevice();
}

void MultiplayerAdapter::onRemoteReadChannelFinished() {
    QIODevice *device = qobject_cast<QIODevice *>(sender());
    Q_ASSERT(device);

    disconnectRemoteClient(device);
}


void MultiplayerAdapter::disconnectRemoteClient(QIODevice *device) {
    return disconnectRemoteClient(m_remote[device]);
}

void MultiplayerAdapter::disconnectRemoteClient(PlayerInfo *info) {
    Q_ASSERT(info);

    if (info->player != NULL)
        info->player->setState(Player::Disconnected);

    m_remote.remove(info->device);

    info->device->deleteLater();
    delete info;
}

void MultiplayerAdapter::setLocalDevice(QIODevice *device) {
    if (device == m_local->device)
        return;

    if (m_local->device)
        m_local->device->deleteLater();

    m_local->device = device;

    if (device) {
        connect(device, SIGNAL(readChannelFinished()), SLOT(onLocalReadChannelFinished()));
        connect(device, SIGNAL(readyRead()), SLOT(onLocalReadyRead()));

        m_local->state = PlayerInfo::Connecting;
    } else {
        m_local->state = PlayerInfo::Disconnected;
    }
}

QIODevice *MultiplayerAdapter::localDevice() const {
    return m_local->device;
}

void MultiplayerAdapter::disconnectLocalDevice(const QString &reason) {
    if (m_local->state == PlayerInfo::Connecting) {
        QString joinErrorReason = reason;

        if (joinErrorReason.isEmpty() && m_local->device)
            joinErrorReason = m_local->device->errorString();

        emit joinFailed(joinErrorReason);
    }

    m_local->state = PlayerInfo::Disconnected;

    if (m_local->device == NULL)
        return;

    if (m_local->device->isOpen())
        m_local->device->close();
}

void MultiplayerAdapter::sendMessage(const PlayerInfo *info, Message *message, PlayerStateFilter stateFilter) {
    qDebug() << "Sending message with type: " << message->type();

    if (info == NULL) {
        foreach(QIODevice *device, m_remote.keys()) {
            PlayerInfo *playerInfo = m_remote[device];

            if (!(playerInfo->state & stateFilter))
                continue;

            QDataStream stream(device);
            if (!message->writeStream(stream))
                qWarning() << "Failed to send message to " << stream.device() << "(" << stream.device()->errorString() << ")";
        }

        if (m_local->device != NULL && m_local->device->isWritable()) {
            QDataStream stream(m_local->device);
            if (!message->writeStream(stream))
                qWarning() << "Failed to send message to " << stream.device() << "(" << stream.device()->errorString() << ")";
        }
    } else {
        QDataStream stream(info->device);
        if (!message->writeStream(stream))
            qWarning() << "Failed to send message to " << stream.device() << "(" << stream.device()->errorString() << ")";
    }
}

GameInfoModel::GameInfoModel(QObject *parent) :
    QAbstractListModel(parent), m_state(Discovering) {

    QHash<int, QByteArray> roles;

    roles[NameRole] = "name";
    roles[PlayerCountRole] = "playerCount";
    roles[GameInfoRole] = "gameInfo";

    setRoleNames(roles);
}

void GameInfoModel::appendGameInfo(GameInfo *gameInfo) {
    quint16 rowCount = m_gameInfoList.size();

    beginInsertRows(QModelIndex(), rowCount, rowCount);

    m_gameInfoList.append(gameInfo);

    endInsertRows();
}

void GameInfoModel::setState(State state) {
    if (state == m_state)
        return;

    m_state = state;

    emit stateChanged();
}

QVariant GameInfoModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_gameInfoList.size())
        return QVariant();

    switch (role) {
    case NameRole:
        return QVariant::fromValue(m_gameInfoList[index.row()]->name());
    case PlayerCountRole:
        return 0;
    case GameInfoRole:
        return QVariant::fromValue(m_gameInfoList[index.row()]);
    }

    return QVariant();
}

int GameInfoModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return m_gameInfoList.size();
}

GameInfo *GameInfoModel::row(int index) const {
    return m_gameInfoList[index];
}
