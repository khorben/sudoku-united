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

#include "player.h"

Player::Player(QObject *parent) :
    QObject(parent), m_state(Connected), m_colorIndex(0)
{
    m_uuid = QUuid::createUuid();
}

Player::Player(const QUuid &uuid, const QString &name, QObject *parent)
    : QObject(parent), m_name(name), m_uuid(uuid), m_state(Connected), m_colorIndex(0)
{
}

Player::Player(const Player &other, QObject *parent) :
    QObject(parent), m_name(other.m_name), m_uuid(other.m_uuid),
    m_state(other.m_state), m_colorIndex(other.m_colorIndex) {

}

void Player::setName(const QString &name) {
    if (m_name == name)
        return;

    m_name = name;

    emit nameChanged();
}

void Player::setState(State state) {
    if (m_state == state)
        return;

    m_state = state;

    emit stateChanged();
}

bool Player::operator ==(const Player &other) const {
    return other.m_uuid == m_uuid;
}
