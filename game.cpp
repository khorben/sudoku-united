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

#include "game.h"
#include "board.h"
#include "player.h"

Game::Game(QObject *parent) :
    QObject(parent), m_board(NULL), m_boardGenerationThread(NULL), m_boardGeneratorWrapper(NULL),
    m_hintGenerationThread(NULL), m_hintGenerator(NULL), m_currentColorIndex(0), m_boardGenerationRunning(false) {
}

Game::Game(Board *board, QObject *parent) :
    QObject(parent), m_board(NULL), m_boardGenerationThread(NULL), m_boardGeneratorWrapper(NULL),
    m_hintGenerationThread(NULL), m_hintGenerator(NULL), m_currentColorIndex(0), m_boardGenerationRunning(false) {
    setBoard(board);
}


void Game::setBoard(Board *board) {
    if (board == m_board)
        return;

    m_board = board;

    if (m_board != NULL)
        m_board->setParent(this);

    emit boardChanged();
}

Player *Game::addPlayer(const QUuid &uuid, const QString &name) {
    Player *player = new Player(uuid, name, this);
    Player *addedPlayer = addPlayer(player);

    if (addedPlayer != player) {
        delete player;

        return addedPlayer;
    }

    return player;
}

Player *Game::addPlayer(Player *player) {
    foreach (Player *existingPlayer, m_players) {
        if (existingPlayer->uuid() == player->uuid()) {
            existingPlayer->setName(player->name());

            return existingPlayer;
        }
    }

    if (m_players.size() >= 0xff)
        return NULL;

    connect(player, SIGNAL(stateChanged()), SLOT(onPlayerStateChanged()));

    player->m_colorIndex = m_currentColorIndex++;

    if (!player->parent())
        player->setParent(this);

    m_players.append(player);

    emit playersChanged();
    emit playerJoined(player);

    return player;
}

QDeclarativeListProperty<Player> Game::playerList() {
    return QDeclarativeListProperty<Player>(this, NULL, &appendPlayersFunction, &countPlayersFunction, &atPlayersFunction);
}

void Game::generateBoard(Sudoku::Difficulty difficulty) {
    if (m_boardGenerationThread != NULL) {
        m_boardGenerationThread->deleteLater();
        m_boardGeneratorWrapper->deleteLater();
    }

    m_boardGenerationThread = new QThread(this);
    m_boardGeneratorWrapper = new BoardGeneratorWrapper(difficulty);

    connect(m_boardGenerationThread, SIGNAL(started()), m_boardGeneratorWrapper, SLOT(startGeneration()));
    connect(m_boardGenerationThread, SIGNAL(finished()), SLOT(onBoardGenerated()));

    m_boardGeneratorWrapper->moveToThread(m_boardGenerationThread);

    m_boardGenerationThread->start();
}

void Game::onBoardGenerated() {
    setBoard(new Board(*m_boardGeneratorWrapper->board()));

    m_boardGenerationThread->deleteLater();
    m_boardGenerationThread = NULL;
    m_boardGeneratorWrapper->deleteLater();
    m_boardGeneratorWrapper = NULL;
}

QList<QObject *> Game::generateHint(){
    QList<QObject *> hint_list;
    quint8 value = 0;
    bool mistakesFound = false;

    //check if there are some mistakes in the board
    for (int i=0; i< 81; i++) {
        value = m_board->cellValue(i % 9, i / 9);
        //if a value is set, it has to be the same as its corresponding element of the solution
        if (value != 0){
            //found a mistake
            if (value != m_board->solutionValue(i % 9, i / 9)){
                mistakesFound = true;
                Cell *cell = m_board->cellAt(i % 9, i / 9);
                hint_list.append(cell);
            }
        }
    }
    // return if mistakes have been found
    if (mistakesFound) {
        return hint_list;
    }

    m_boardGenerationRunning = true;
    emit hintGenerationRunningChanged();

    if (m_hintGenerationThread != NULL) {
        m_hintGenerationThread->deleteLater();
        m_hintGenerator->deleteLater();
    }

    m_hintGenerationThread = new QThread(this);
    m_hintGenerator = new HintGenerator(m_board);

    connect(m_hintGenerationThread, SIGNAL(started()), m_hintGenerator, SLOT(startHintGeneration()));
    connect(m_hintGenerationThread, SIGNAL(finished()), SLOT(onHintGenerated()));

    m_hintGenerator->moveToThread(m_hintGenerationThread);

    m_hintGenerationThread->start();
    return hint_list;
}

void Game::onHintGenerated() {
    BoardGenerator boardGenerator = m_hintGenerator->boardGenerator();
    for (std::vector<LogItem *>::iterator it(boardGenerator.getSolveInstructions()->begin()); it != boardGenerator.getSolveInstructions()->end(); it++) {
        LogItem *item = *it;
        if (item->getType() == LogItem::GIVEN)
            continue;
        Cell *cell = m_board->cellAt(item->getPosition() % 9, item->getPosition() / 9);
        cell->setValue(item->getValue());
        break;
    }
    m_boardGenerationRunning = false;
    emit hintGenerationRunningChanged();
    m_hintGenerationThread->deleteLater();
    m_hintGenerationThread = NULL;
    m_hintGenerator->deleteLater();
    m_hintGenerator = NULL;
}

void Game::onPlayerStateChanged() {
    Player *player = qobject_cast<Player *>(sender());

    if (!player)
        return;

    switch (player->state()) {
    case Player::Connected:
        emit playerJoined(player);
        break;
    case Player::Disconnected:
        emit playerLeft(player);
        break;
    }
}

void Game::appendPlayersFunction(QDeclarativeListProperty<Player> *property, Player *value) {
    Q_UNUSED(value)

    qmlInfo(property->object) << "Appending players is not supported.";
}

Player *Game::atPlayersFunction(QDeclarativeListProperty<Player> *property, int index) {
    return ((Game *) property->object)->m_players[index];
}

int Game::countPlayersFunction(QDeclarativeListProperty<Player> *property) {
    return ((Game *) property->object)->m_players.size();
}


QDataStream &operator<<(QDataStream &stream, Game &game) {
    // Write encoding version
    stream << quint16(1);

    // Write player list
    QMap<Player *, quint8> playerMap;
    quint8 playerIndex = 0;

    stream << quint8(game.players().size());

    foreach (Player *player, game.players()) {
        stream << player->name();
        stream << player->uuid();

        playerMap[player] = playerIndex;

        playerIndex++;
    }

    // Write board
    Board *board = game.board();

    // Write elapsed time
    stream << board->elapsedTime();

    // Write difficulty
    stream << quint8(board->difficulty());

    // Write the boards current cells
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            Cell *cell = board->cellAt(x, y);
            stream << cell->isFixedCell();
            stream << cell->value();

            if (!cell->isFixedCell() && cell->value() != 0) {
                if (cell->valueOwner())
                    stream << quint8(playerMap[cell->valueOwner()]);
                else
                    stream << quint8(0xff);
            }
        }
    }

    // Write the solution
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            stream << board->m_solution[x][y];
        }
    }

    // Write the game history
    stream << quint32(board->modificationLog.size());
    foreach(ModificationLogEntry entry, board->modificationLog) {
        stream << entry.x();
        stream << entry.y();
        stream << entry.value();
        if (entry.valueOwner())
            stream << playerMap[entry.valueOwner()];
        else
            stream << quint8(0xff);
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, Game &game) {
    // Read version
    quint16 version;
    stream >> version;

    if (version != 1) {
        stream.setStatus(QDataStream::ReadCorruptData);
        return stream;
    }

    // Read player list
    quint8 playerCount;
    QMap<quint8, Player *> playerMap;
    stream >> playerCount;

    for (quint8 playerIndex = 0; playerIndex < playerCount; playerIndex++) {
        QString name;
        QUuid uuid;

        stream >> name;
        stream >> uuid;

        Player *player = game.addPlayer(uuid, name);
        player->setState(Player::Disconnected);

        playerMap[playerIndex] = player;
    }

    // Read board
    Board *board = new Board(&game);

    // Read elapsed time
    stream >> board->m_elapsedTime;
    board->m_startTime = QDateTime::currentMSecsSinceEpoch();

    // Read difficulty
    quint8 difficulty;
    stream >> difficulty;
    board->m_difficulty = (Sudoku::Difficulty) difficulty;

    // Read the boards current cells
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            Cell *cell = board->cellAt(x, y);

            stream >> cell->m_fixedCell;
            stream >> board->m_cellValues[x][y];

            if (!cell->isFixedCell() && cell->value() != 0) {
                quint8 playerIndex;
                stream >> playerIndex;

                if (playerIndex != 0xff)
                    cell->setValueOwner(playerMap[playerIndex]);
            }
        }
    }

    // Read the solution
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            stream >> board->m_solution[x][y];
        }
    }

    // Restore the game history
    quint32 modificationLogLength;
    stream >> modificationLogLength;

    for (quint32 i = 0; i < modificationLogLength; i++) {
        quint8 x, y, value, playerIndex;

        stream >> x;
        stream >> y;
        stream >> value;
        stream >> playerIndex;

        Player *player = NULL;
        if (playerIndex != 0xff)
            player = playerMap[playerIndex];

        board->modificationLog.push(ModificationLogEntry(x, y, value, player));
    }

    game.setBoard(board);

    return stream;
}
