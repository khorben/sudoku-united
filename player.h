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

#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QUuid>
#include <QDataStream>
#include <QtDeclarative>

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(const QString& name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(quint8 colorIndex READ colorIndex CONSTANT)
public:
    enum State {
        Connected,
        Disconnected
    };

    explicit Player(QObject *parent = 0);
    explicit Player(const QUuid &uuid, const QString &name, QObject *parent = 0);

    State state() const { return m_state; }
    void setState(State state);

    const QString &name() const { return m_name; }
    void setName(const QString &name);

    const QUuid &uuid() const { return m_uuid; }

    quint8 colorIndex() const { return m_colorIndex; }

    bool operator ==(const Player &other) const;
signals:
    void nameChanged();
    void stateChanged();
public slots:

private:
    friend class JoinMessage;
    friend class Game;

    QString m_name;
    QUuid m_uuid;
    State m_state;
    quint8 m_colorIndex;
};

QML_DECLARE_TYPE(Player)

#endif // PLAYER_H
