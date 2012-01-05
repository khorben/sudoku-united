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

#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <QObject>
#include <QtDeclarative>
#include <QAbstractListModel>

class AbstractClient;

class GameInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
public:
    GameInfo(QObject *parent = 0);

    /**
      * \return The name of this game.
      */
    virtual QString name() const = 0;

    /**
      * \return A client adapter which is able to join this game. The caller
      * of this function must take ownership of the returned value.
      */
    virtual AbstractClient *client() const = 0;

    /**
      * Override the equality operator to indicate if two GameInfo entries
      * contain the same values.
      */
    virtual bool operator ==(GameInfo &other) const = 0;
};

QML_DECLARE_TYPE(GameInfo)

class GameInfoModel : public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool autoRefresh READ autoRefresh WRITE setAutoRefresh NOTIFY
               autoRefreshChanged)
public:
    enum State {
        Discovering,
        Complete
    };

    State state() const { return m_state; }

    virtual QVariant data(const QModelIndex &index, int role) const;

    int rowCount(const QModelIndex &parent) const;

    GameInfo *row(int index) const;

    virtual void setAutoRefresh(bool enabled);
    bool autoRefresh() const;
signals:
    void stateChanged();
    void autoRefreshChanged();
protected:
    GameInfoModel(QObject *parent = 0);

    int insertGameInfo(GameInfo *gameInfo);

    void setState(State state);
protected:
    QList<GameInfo *> m_gameInfoList;
    State m_state;
private:
    bool m_autoRefresh;
private:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PlayerCountRole,
        GameInfoRole
    };
};

QML_DECLARE_TYPE(GameInfoModel)

#endif // GAMEINFO_H
