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

class MultiplayerAdapter;
class Player;

class Sudoku : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MultiplayerAdapter * multiplayerAdapter READ multiplayerAdapter CONSTANT)
    Q_PROPERTY(Player * player READ player CONSTANT)
public:
    explicit Sudoku(QObject *parent = 0);

    MultiplayerAdapter *multiplayerAdapter() const { return m_multiplayerAdapter; }

    Player *player() const { return m_player; }

    static Sudoku *instance();
signals:

public slots:

private:
    MultiplayerAdapter *m_multiplayerAdapter;
    Player *m_player;

private:
    static Sudoku *m_instance;

};

QML_DECLARE_TYPE(Sudoku)

#endif // SUDOKU_H
