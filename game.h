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

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QDeclarativeListProperty>

#include "boardgenerator.h"

class Board;
class Player;
class QUuid;

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Board *board READ board NOTIFY boardChanged)
    Q_PROPERTY(QDeclarativeListProperty<Player> players READ playerList NOTIFY playersChanged)
public:
    explicit Game(QObject *parent = 0);
    explicit Game(Board *board, QObject *parent = 0);

    Board *board() const { return m_board; }
    void setBoard(Board *board);

    const QList<Player *> &players() const { return m_players; }

    QDeclarativeListProperty<Player> playerList();

    /**
      * Adds a player to this game.
      */
    Player *addPlayer(const QUuid &uuid, const QString &name);

    Player *addPlayer(Player *player);

    /**
      * Generates a new playing board asynchronously.
      */
    void generateBoard(BoardGenerator::Difficulty difficulty = BoardGenerator::EASY);
signals:
    /**
      * Indicates that a new playing board has been set.
      */
    void boardChanged();

    /**
      * Indicates that the list of players has been changed.
      */
    void playersChanged();
private slots:
    void onBoardGenerated();
private:
    Board *m_board;
    QList<Player *> m_players;
    QThread *m_boardGenerationThread;
    BoardGeneratorWrapper *m_boardGeneratorWrapper;
    quint8 m_currentColorIndex;
private:
    static int countPlayersFunction(QDeclarativeListProperty<Player> *property);
    static Player *atPlayersFunction(QDeclarativeListProperty<Player> *property, int index);
    static void appendPlayersFunction(QDeclarativeListProperty<Player> *property, Player *value);
};

#endif // GAME_H
