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

#include "board.h"
#include "player.h"
#include "boardgenerator.h"

Board::Board(const Board &other) :
    QObject(other.parent()) {

    memcpy(m_cellValues, other.m_cellValues, sizeof(m_cellValues));

    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            m_cells[x][y] = other.m_cells[x][y];
            m_cells[x][y].setParent(this);
            m_solution[x][y] = other.m_solution[x][y];

            connect(&m_cells[x][y], SIGNAL(valueChanged()), SLOT(onCellValueChanged()));
        }
    }

    m_startTime = other.m_startTime;
    m_elapsedTime = other.m_elapsedTime;
    m_paused = other.m_paused;
}

Board::Board(QObject *parent) :
    QObject(parent)
{
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            m_cellValues[x][y] = 0;
            m_cells[x][y].m_x = x;
            m_cells[x][y].m_y = y;
            m_cells[x][y].setParent(this);

            connect(&m_cells[x][y], SIGNAL(valueChanged()), SLOT(onCellValueChanged()));
        }
    }

    m_startTime = QDateTime::currentMSecsSinceEpoch();
    m_elapsedTime = 0;
    m_paused = false;
}

Cell *Board::cellAt(quint8 x, quint8 y) const {
    if (x >= 9 || y >= 9) {
        qmlInfo(this) << "Invalid cell index specified.";

        return NULL;
    }

    return const_cast<Cell *>(&m_cells[x][y]);
}

void Board::onCellValueChanged() {
    Cell *cell = qobject_cast<Cell *>(sender());

    emit cellValueChanged(cell);

    if (isFull())
        emit boardIsFull();
}

QList<QObject *> Board::isValidMove(quint8 x, quint8 y, quint8 value) {
    QList<QObject *> list;

    isValidMoveHorizontal(x, y, value, list);
    isValidMoveVertical(x, y, value, list);
    isValidMoveBlock(x, y, value, list);

    return list;
}

void Board::isValidMoveHorizontal(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    for (quint8 i = 0; i < 9; i++) {
        if (i == x)
            continue;

        Cell *cell = cellAt(i, y);

        if (cell->value() == val && !invalidList.contains(cell))
            invalidList.append(cell);
    }
}

void Board::isValidMoveVertical(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    for (quint8 i = 0; i < 9; i++) {
        if (i == y)
            continue;

        Cell *cell = cellAt(x, i);

        if (cell->value() == val && !invalidList.contains(cell))
            invalidList.append(cell);
    }
}

void Board::isValidMoveBlock(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const {
    quint8 startX = quint8(x / 3) * 3;
    quint8 startY = quint8(y / 3) * 3;

    for (quint8 yR = startY; yR < startY + 3; yR++) {
        for (quint8 xR = startX; xR < startX + 3; xR++) {
            if (xR == x && yR == y)
                continue;

            Cell *cell = cellAt(xR, yR);

            if (cell->value() == val && !invalidList.contains(cell))
                invalidList.append(cell);
        }
    }
}

bool Board::isFull() const {
    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            if (cellValue(x, y) == 0)
                return false;
        }
    }

    return true;
}

QString Board::toString() const {
    QString output;

    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            output += QString("| ") + QString::number(cellValue(x, y)) + QString(" ");
        }
        output += " |\n";
        output += "|------------------------------------|\n";
    }

    return output;
}
void Board::clear() {
    for (quint8 x = 0; x < 9; x++) {
        for (quint8 y = 0; y < 9; y++) {
            cellAt(x, y)->clear();
        }
    }
}

quint8 Board::cellValue(quint8 x, quint8 y) const {
    return m_cellValues[x][y];
}

quint8 Board::solutionValue(quint8 x, quint8 y) const {
    return m_solution[x][y];
}

void Board::setCellValue(quint8 x, quint8 y, quint8 value) {
    m_cellValues[x][y] = value;
}
quint64 Board::startTime() const {
    return m_startTime;
}

quint64 Board::elapsedTime() const {
    // If we are paused return the elapsed time. The WindowActived event
    // is delivered after the elapsed time is requested which leads to
    // an incorrect display of the elapsed time.
    if (m_paused)
        return m_elapsedTime;

    return ((QDateTime::currentMSecsSinceEpoch() - m_startTime) + m_elapsedTime);
}

void Board::pause() {
    m_paused = true;
    m_elapsedTime += QDateTime::currentMSecsSinceEpoch() - m_startTime;
}

void Board::unpause() {
    if (!m_paused)
        return;

    m_paused = false;
    m_startTime = QDateTime::currentMSecsSinceEpoch();
}

QDataStream &operator<<(QDataStream &s, const Board &board) {
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            Cell *cell = board.cellAt(x, y);

            QUuid uuid;

            if (cell->valueOwner() != NULL)
                uuid = cell->valueOwner()->uuid();

            s << uuid;
            s << cell->isFixedCell();
            s << cell->value();
        }
    }

    return s;
}

QDataStream &operator>>(QDataStream &s, Board &board) {
    for (quint8 y = 0; y < 9; y++) {
        for (quint8 x = 0; x < 9; x++) {
            QUuid uuid;

            s >> uuid;
            s >> board.cellAt(x, y)->m_fixedCell;
            s >> board.m_cellValues[x][y];
        }
    }

    return s;
}

Cell::Cell(quint8 x, quint8 y, Board *parent) :
    QObject(parent), m_x(x), m_y(y), m_fixedCell(false), m_valueOwner(NULL) {
}

quint8 Cell::value() const {
    return board()->m_cellValues[m_x][m_y];
}

void Cell::setValue(quint8 value) {
    if (this->value() == value)
        return;

    board()->setCellValue(m_x, m_y, value);

    if (value == 0)
        setValueOwner(NULL);

    emit valueChanged();
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

Cell &Cell::operator =(const Cell &other) {
    m_x = other.m_x;
    m_y = other.m_y;
    m_fixedCell = other.m_fixedCell;
    m_valueOwner = other.m_valueOwner;

    return *this;
}
