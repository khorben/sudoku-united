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

#ifndef CELL_H
#define CELL_H

#include <QObject>
#include <QtDeclarative>

class NoteModel;
class Board;
class Player;
class Game;

class Cell : public QObject {
    Q_OBJECT
    Q_PROPERTY(quint8 value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Player *valueOwner READ valueOwner WRITE setValueOwner NOTIFY valueOwnerChanged)
    Q_PROPERTY(quint8 x READ x CONSTANT)
    Q_PROPERTY(quint8 y READ y CONSTANT)
    Q_PROPERTY(quint8 block READ block CONSTANT)
    Q_PROPERTY(NoteModel *noteModel READ noteModel CONSTANT)

public:
    Cell();
    explicit Cell(quint8 x, quint8 y, Board *parent = 0);

    Cell &operator =(const Cell &other);

    quint8 value() const;
    void setValue(quint8 value);

    Player *valueOwner() const { return m_valueOwner; }
    void setValueOwner(Player *player);

    Q_INVOKABLE
    bool isFixedCell() const { return m_fixedCell; }

    void clear();

    quint8 x() const { return m_x; }
    quint8 y() const { return m_y; }
    quint8 block() const { return quint8(m_y / 3) * 3 + quint8(m_x / 3); }

    NoteModel *noteModel() const;
signals:
    void beforeValueChanged();
    void valueChanged();
    void valueOwnerChanged();
private:
    friend class SudokuBoard;
    friend class BoardGenerator;
    friend class GameMessage;
    friend QDataStream &readGameV1(QDataStream &stream, Game &game);
    friend QDataStream &readGameV2(QDataStream &stream, Game &game);

    inline Board *board() const { return (Board *) parent(); }
    friend class Board;

    quint8 m_x;
    quint8 m_y;
    bool m_fixedCell;
    Player *m_valueOwner;
    NoteModel *m_noteModel;
};

QML_DECLARE_TYPE(Cell)

#endif // CELL_H
