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

#include "cell.h"
#include "notemodel.h"
#include "board.h"

Cell::Cell(quint8 x, quint8 y, Board *parent) :
    QObject(parent), m_x(x), m_y(y), m_fixedCell(false), m_valueOwner(NULL),
    m_noteModel(new NoteModel(this)) {
}

quint8 Cell::value() const {
    return board()->m_cellValues[m_x][m_y];
}

void Cell::setValue(quint8 value) {
    board()->setCellValue(m_x, m_y, value);
}

void Cell::setValueOwner(Player *player) {
    if (player == m_valueOwner || (player && m_valueOwner && player == m_valueOwner))
        return;

    m_valueOwner = player;

    emit valueOwnerChanged();
}

void Cell::clear() {
    setValue(0);
    m_valueOwner = NULL;
}

NoteModel *Cell::noteModel() const
{
    return m_noteModel;
}

Cell &Cell::operator =(const Cell &other) {
    m_x = other.m_x;
    m_y = other.m_y;
    m_fixedCell = other.m_fixedCell;
    m_valueOwner = other.m_valueOwner;

    return *this;
}

Cell::Cell() :
    m_fixedCell(false), m_valueOwner(NULL), m_noteModel(new NoteModel(this))
{
}
