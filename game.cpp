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
