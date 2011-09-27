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

#include "sudoku.h"
#include "player.h"
#include "adapters/serveradapter.h"
#include "adapters/abstractserver.h"
#include "adapters/abstractclient.h"
#include "adapters/tcp/tcpserver.h"
#include "adapters/bluetooth/bluetoothserver.h"
#include "adapters/telepathy/telepathyserver.h"
#include "game.h"

Sudoku *Sudoku::m_instance = NULL;

Sudoku::Sudoku(QObject *parent) :
    QObject(parent), m_player(NULL), m_game(NULL),
    serverAdapter(new ServerAdapter(this)), client(NULL)
{
    m_settings = new Settings(this);
    connect(m_settings, SIGNAL(playerNameChanged()),
            SLOT(onPlayerNameChanged()));

    m_player = new Player(this);
    m_player->setName(m_settings->playerName());

    serverAdapter->addServerImplementation(new TCPServer());
    serverAdapter->addServerImplementation(new TelepathyServer());
    if (m_settings->bluetoothEnabled())
        serverAdapter->addServerImplementation(new BluetoothServer());
}

Settings *Sudoku::settings() const {
    return m_settings;
}

void Sudoku::onPlayerNameChanged() {
    m_player->setName(m_settings->playerName());
}

Sudoku *Sudoku::instance() {
    if (m_instance == NULL)
        m_instance = new Sudoku();

    return m_instance;
}

void Sudoku::setGame(Game *game) {
    if (game == m_game)
        return;

    if (m_game)
        m_game->deleteLater();

    m_game = game;

    if (m_game) {
        m_game->setParent(this);
    } else if (client) {
        client->leave();
        client->deleteLater();
        client = NULL;
    }

    serverAdapter->setGame(m_game);

    emit gameChanged();
}

void Sudoku::join(GameInfo *game) {
    setGame(NULL);

    // Variable to store whether we have reached the connected state
    notConnected = true;
    client = game->client();
    connect(client,
            SIGNAL(stateChanged(AbstractClient::State)),
            SLOT(onClientStateChanged(AbstractClient::State)));
    client->join(game);
}

void Sudoku::onClientStateChanged(AbstractClient::State state) {
    switch (state) {
    case AbstractClient::Disconnected:
        if (m_game == NULL && notConnected)
            emit joinFailed(client->error());
        break;
    case AbstractClient::Connected:
        setGame(client->game());
        notConnected = false;
        break;
    case AbstractClient::Connecting:
        break;
    }
}

void Sudoku::cancelJoin() {
    if (client)
        client->leave();
}

GameInfoModel *Sudoku::discoverGames() {
    return new AggregateGameInfoModel(this);
}

Game *Sudoku::createGame(Difficulty difficulty) {
    if (m_game)
        m_game->deleteLater();

    Game *game = new Game(this);
    game->addPlayer(m_player);

    setGame(game);

    game->generateBoard(difficulty);

    return m_game;
}

void Sudoku::leave() {
    if (!m_game)
        return;

    setGame(NULL);
}


AggregateGameInfoModel::AggregateGameInfoModel(Sudoku *parent) :
    GameInfoModel(parent) {

    foreach (AbstractServer *adapter,
             parent->serverAdapter->serverImplementations()) {
        GameInfoModel *model = adapter->discoverGames();
        rowMap[model] = QHash<quint16, quint16>();

        m_gameInfoModels.append(model);

        QHash<quint16, quint16> &rowInfo = rowMap[model];
        for (int i = 0; i < model->rowCount(QModelIndex()); i++) {
            insertGameInfo(model->row(i));
            rowInfo[i] = m_gameInfoList.size() - 1;

        }

        connect(model, SIGNAL(stateChanged()), SLOT(onStateChanged()));
        connect(model,
                SIGNAL(rowsInserted(QModelIndex, int, int)),
                SLOT(onRowsInserted(QModelIndex,int,int)));
        connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                SLOT(onRowsRemoved(QModelIndex, int, int)));
        connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                SLOT(onDataChanged(QModelIndex, QModelIndex)));
    }
}

void AggregateGameInfoModel::onRowsInserted(const QModelIndex &parent,
                                            int start, int end) {
    Q_UNUSED(parent)

    GameInfoModel *model = (GameInfoModel *) sender();
    QHash<quint16, quint16> &rowInfo = rowMap[model];

    for (int i = start; i <= end; i++) {
        rowInfo[i] = insertGameInfo(model->row(i));
    }
}

void AggregateGameInfoModel::onRowsRemoved(const QModelIndex &parent, int start,
                                           int end) {
    Q_UNUSED(parent)

    GameInfoModel *model = (GameInfoModel *) sender();
    QHash<quint16, quint16> &rowInfo = rowMap[model];

    for (int i = start; i <= end; i++) {
        beginRemoveRows(QModelIndex(), rowInfo[i], rowInfo[i]);
        m_gameInfoList.removeAt(rowInfo[i]);
        endRemoveRows();
    }

    // Rebuild row mapping
    rowInfo.clear();
    for (int i = 0; i < model->rowCount(QModelIndex()); i++) {
        for (int z = 0; z < m_gameInfoList.size(); z++) {
            if (*(model->row(i)) == *(m_gameInfoList[z])) {
                rowInfo[i] = z;
                break;
            }
        }
    }
}

void AggregateGameInfoModel::onDataChanged(const QModelIndex &topLeft,
                                           const QModelIndex &bottomRight) {
    GameInfoModel *model = (GameInfoModel *) sender();
    QHash<quint16, quint16> &rowInfo = rowMap[model];

    for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
        QModelIndex index = createIndex(rowInfo[row], 0);
        emit dataChanged(index, index);
    }
}

void AggregateGameInfoModel::onStateChanged() {
    foreach (GameInfoModel *model, m_gameInfoModels) {
        if (model->state() == Discovering) {
            if (m_state != model->state()) {
                m_state = model->state();
                emit stateChanged();
            }
            return;
        }
    }

    m_state = Complete;
    emit stateChanged();
}

void AggregateGameInfoModel::setAutoRefresh(bool enabled) {
    GameInfoModel::setAutoRefresh(enabled);

    foreach (GameInfoModel *model, m_gameInfoModels)
        model->setAutoRefresh(enabled);
}
