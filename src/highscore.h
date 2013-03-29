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

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <QString>
#include <QtDeclarative>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include "sudoku.h"

class HighscoreEntry;

#define MAX_ENTRIES 10

class HighscoreModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(bool empty READ isEmpty NOTIFY emptyChanged)

public:
    HighscoreModel(QObject *parent);
    virtual QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    Q_INVOKABLE
    void addHighscore(QList<Player *> players, quint64 playTime, Sudoku::Difficulty difficulty, QDateTime finishedDate);

    QList<HighscoreEntry *> highscores(){ return m_highscoreList; }

    bool isEmpty() const;

    Q_INVOKABLE
    void clear();

public:
    enum Roles {
        PlayTimeRole = Qt::UserRole + 1,
        DifficultyRole,
        NumberOfPlayersRole,
        DifficultyStringRole,
        SectionIndexRole,
        PlayerNamesRole,
        FinishedDateRole
    };

signals:
    void emptyChanged();

private:
    QList<HighscoreEntry *> m_highscoreList;
};

QDataStream &operator<<(QDataStream &stream, HighscoreModel &highscore);
QDataStream &operator>>(QDataStream &stream, HighscoreModel &highscore);

QML_DECLARE_TYPE(HighscoreModel)

class HighscoreEntry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 numberOfPlayers READ numberOfPlayers CONSTANT)
    Q_PROPERTY(quint64 playTime READ playTime CONSTANT)
    Q_PROPERTY(Sudoku::Difficulty difficulty READ difficulty CONSTANT)

public:
    HighscoreEntry(QObject *parent = 0);
    HighscoreEntry(const QList<Player *> &players, quint64 playTime, Sudoku::Difficulty difficulty, QDateTime finishedDate, QObject *parent = 0);

    quint8 numberOfPlayers() const { return m_players.size(); }
    quint64 playTime() const { return m_playTime; }
    Sudoku::Difficulty difficulty() const { return m_diffculty; }
    QList<Player *> players() const { return m_players; }
    QDateTime finishedDate() const { return m_finishedDate; }

private:
    QList<Player *> m_players;
    quint64 m_playTime;
    Sudoku::Difficulty m_diffculty;
    QDateTime m_finishedDate;
};

class HighscoreFilterModel : public QSortFilterProxyModel {
public:
    HighscoreFilterModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // HIGHSCORE_H
