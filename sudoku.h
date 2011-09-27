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

#include "settings.h"
#include "adapters/abstractclient.h"
#include "adapters/gameinfo.h"

class Player;
class Game;

class ServerAdapter;
class GameInfoModel;

class Sudoku : public QObject
{
    Q_OBJECT
    Q_ENUMS(Difficulty)
    Q_PROPERTY(Player * player READ player CONSTANT)
    Q_PROPERTY(Game * game READ game NOTIFY gameChanged)
    Q_PROPERTY(Settings *settings READ settings CONSTANT)
public:
    enum Difficulty {
        SIMPLE,
        EASY,
        INTERMEDIATE,
        HARD,
        EXPERT
    };

public:
    explicit Sudoku(QObject *parent = 0);

    Player *player() const { return m_player; }

    Game *game() const { return m_game; }

    Q_INVOKABLE
    virtual void join(GameInfo *game);

    Q_INVOKABLE
    virtual GameInfoModel *discoverGames();

    Q_INVOKABLE
    virtual Game *createGame(Difficulty difficulty);

    Q_INVOKABLE
    virtual void leave();

    Q_INVOKABLE
    virtual void cancelJoin();

    Settings *settings() const;

    static Sudoku *instance();
signals:
    void joinFailed(QString reason);
    void gameChanged();
public slots:

protected slots:
    void onClientStateChanged(AbstractClient::State state);
    void setGame(Game *game);
protected:
    bool eventFilter(QObject *filterObj, QEvent *evt);
private slots:
    void onPlayerNameChanged();
private:
    friend class AggregateGameInfoModel;

    Player *m_player;
    Game *m_game;
    ServerAdapter *serverAdapter;
    AbstractClient *client;
    Settings *m_settings;
    bool notConnected;
private:
    static Sudoku *m_instance;

};

QML_DECLARE_TYPE(Sudoku)

class AggregateGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    AggregateGameInfoModel(Sudoku *parent);

    void setAutoRefresh(bool enabled);
private slots:
    void onRowsInserted(const QModelIndex & parent, int start, int end);
    void onRowsRemoved(const QModelIndex &parent, int start, int end);
    void onDataChanged(const QModelIndex &topLeft,
                       const QModelIndex &bottomRight);
    void onStateChanged();
private:
    QList<GameInfoModel *> m_gameInfoModels;
    QHash<GameInfoModel *, QHash<quint16, quint16> > rowMap;
};

#endif // SUDOKU_H
