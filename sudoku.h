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

#ifndef SUDOKU_H
#define SUDOKU_H

#include <QObject>
#include <QtDeclarative>

#include "boardgenerator.h"
#include "multiplayeradapter.h"

class Player;
class Game;

class Sudoku : public QObject
{
    Q_OBJECT
    // Q_PROPERTY(MultiplayerAdapter * multiplayerAdapter READ multiplayerAdapter CONSTANT)
    Q_PROPERTY(Player * player READ player CONSTANT)
    Q_PROPERTY(Game * game READ game NOTIFY gameChanged)
public:
    explicit Sudoku(QObject *parent = 0);

    Player *player() const { return m_player; }

    Game *game() const { return m_game; }

    Q_INVOKABLE
    virtual void join(GameInfo *game);

    Q_INVOKABLE
    virtual GameInfoModel *discoverGames();

    Q_INVOKABLE
    virtual Game *createGame(BoardGenerator::Difficulty difficulty = BoardGenerator::EASY);

    Q_INVOKABLE
    virtual void leave();

    Q_INVOKABLE
    virtual void cancelJoin();

    static Sudoku *instance();
signals:
    void joinFailed(QString reason);
    void gameChanged();
public slots:

protected slots:
    void setGame(Game *game);
private slots:
    void onJoinFailed(const QString &reason);
private:
    void addMultiplayerAdapter(MultiplayerAdapter *adapter);

private:
    friend class AggregateGameInfoModel;

    QList<MultiplayerAdapter *> m_multiplayerAdapters;
    Player *m_player;
    Game *m_game;
    MultiplayerAdapter *joinAdapter;
private:
    static Sudoku *m_instance;

};

QML_DECLARE_TYPE(Sudoku)

class AggregateGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    AggregateGameInfoModel(Sudoku *parent);

private slots:
    void onRowsInserted(const QModelIndex & parent, int start, int end);
    void onStateChanged();
private:
    QList<GameInfoModel *> m_gameInfoModels;
};

#endif // SUDOKU_H
