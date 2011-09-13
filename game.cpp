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
    QObject(parent), m_board(NULL), m_boardGenerationThread(NULL), m_boardGeneratorWrapper(NULL), m_currentColorIndex(0)
{
}

Game::Game(Board *board, QObject *parent) :
    QObject(parent), m_board(NULL), m_boardGenerationThread(NULL), m_boardGeneratorWrapper(NULL), m_currentColorIndex(0) {
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

    player->m_colorIndex = m_currentColorIndex++;

    if (!player->parent())
        player->setParent(this);

    m_players.append(player);

    emit playersChanged();

    return player;
}

QDeclarativeListProperty<Player> Game::playerList() {
    return QDeclarativeListProperty<Player>(this, NULL, &appendPlayersFunction, &countPlayersFunction, &atPlayersFunction);
}

void Game::generateBoard(BoardGenerator::Difficulty difficulty) {
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
