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
#include "board.h"

class Player;
class QUuid;

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Board *board READ board NOTIFY boardChanged)
    Q_PROPERTY(QDeclarativeListProperty<Player> players READ playerList NOTIFY playersChanged)
    Q_PROPERTY(bool generationRunning READ hintGenerationRunning NOTIFY hintGenerationRunningChanged)
public:
    explicit Game(QObject *parent = 0);
    explicit Game(Board *board, QObject *parent = 0);

    Board *board() const { return m_board; }
    void setBoard(Board *board);

    const QList<Player *> &players() const { return m_players; }

    bool hintGenerationRunning() const { return m_boardGenerationRunning; }

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

    /**
      * Generate a hint for the current board
      */
    Q_INVOKABLE
    QList<QObject *> generateHint();

signals:
    /**
      * Indicates that a new playing board has been set.
      */
    void boardChanged();

    /**
      * Indicates that the list of players has been changed.
      */
    void playersChanged();

    /**
      * Convenience signal which is emitted when a new player joins the game.
      */
    void playerJoined(Player *player);

    /**
      * Convenience signal which is emitted when a player leaves the game.
      */
    void playerLeft(Player *player);

    /**
      * Indicates that the state of the hint generation has changed.
      */
    void hintGenerationRunningChanged();

private slots:
    void onBoardGenerated();
    void onHintGenerated();
    void onPlayerStateChanged();
private:
    Board *m_board;
    QList<Player *> m_players;
    QThread *m_boardGenerationThread;
    BoardGeneratorWrapper *m_boardGeneratorWrapper;
    QThread *m_hintGenerationThread;
    HintGenerator *m_hintGenerator;
    quint8 m_currentColorIndex;
    bool m_boardGenerationRunning;
private:
    static int countPlayersFunction(QDeclarativeListProperty<Player> *property);
    static Player *atPlayersFunction(QDeclarativeListProperty<Player> *property, int index);
    static void appendPlayersFunction(QDeclarativeListProperty<Player> *property, Player *value);
};

#endif // GAME_H
