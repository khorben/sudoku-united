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

#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QtDeclarative>
#include <QPair>
#include <QDeclarativeListProperty>

#include "sudoku.h"
#include "cell.h"

class QDataStream;
class Player;
class Game;

class ModificationLogEntry {
public:
    ModificationLogEntry();
    ModificationLogEntry(const Cell *cell);
    ModificationLogEntry(quint8 x, quint8 y, quint8 value, Player *valueOwner);

    quint8 x() const;
    quint8 y() const;
    quint8 value() const;
    Player *valueOwner() const;
private:
    quint8 m_x;
    quint8 m_y;
    quint8 m_value;
    Player *m_valueOwner;
};

class Board : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool full READ isFull NOTIFY boardIsFull)
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY canUndoChanged)
    Q_PROPERTY(quint64 startTime READ startTime CONSTANT)
    Q_PROPERTY(quint32 elapsedTime READ elapsedTime)
    Q_PROPERTY(Cell * selectedCell READ selectedCell WRITE setSelectedCell NOTIFY selectedCellChanged)
public:
    explicit Board(QObject *parent = 0);
    Board(const Board &other, QObject *parent = 0);

    /**
      * Returns a Cell object describing the cell at the
      * given coordinates.
      *
      * May return NULL if the coordinates were invalid.
      */
    Q_INVOKABLE
    Cell *cellAt(quint8 x, quint8 y) const;


    /**
      * Checks if the given move is failed. Returns an empty list
      * if it is. If it isn't a list of conflicting fields is
      * returned.
      */
    Q_INVOKABLE
    QList<QObject *> isValidMove(quint8 x, quint8 y, quint8 value);

    /**
      * Clears the board resetting all values to their default.
      */
    void clear();

    /**
      * \returns True, if all fields of the board are set, false otherwise.
      */
    Q_INVOKABLE
    bool isFull() const;

    /**
      * \returns True, if all fields of the row are set, false otherwise.
      */
    Q_INVOKABLE
    bool isRowFull(int row) const;

    /**
      * \returns True, if all fields of the column are set, false otherwise.
      */
    Q_INVOKABLE
    bool isColumnFull(int column) const;

    /**
      * \returns True, if all fields of the block are set, false otherwise.
      */
    Q_INVOKABLE
    bool isBlockFull(int block) const;

    /**
      * \returns True, if all instances of the value are set, false otherwise.
      */
    Q_INVOKABLE
    bool isValueFull(int value) const;

    /**
      * Creates a string representation of the current board.
      */
    QString toString() const;

    /**
      * Gets the value of the cell at coordinates x, y.
      *
      * \returns The value of the cell or 0 if it is not set.
      */
    quint8 cellValue(quint8 x, quint8 y) const;

    /**
      * Gets the value of the cell at coordinates x, y.
      *
      * \returns The value of the cell or 0 if it is not set.
      */
    quint8 solutionValue(quint8 x, quint8 y) const;

    quint64 startTime() const;

    quint64 elapsedTime() const;

    Cell *selectedCell() const;
    void setSelectedCell(Cell *cell);

    Q_INVOKABLE
    void pause();

    Q_INVOKABLE
    void unpause();

    Q_INVOKABLE
    void undo();

    bool canUndo() const;

    Sudoku::Difficulty difficulty() const;
signals:
    /**
      * This signal is emitted if the value of a cell changes.
      */
    void cellValueChanged(Cell *cell);

    void selectedCellChanged();

    void boardIsFull();
    void rowIsFull(int row);
    void columnIsFull(int column);
    void blockIsFull(int block);
    void valueIsFull(int value);

    void canUndoChanged();
public slots:

private slots:
    void onBeforeCellValueChanged();
    void onCellValueChanged();
private:
    void isValidMoveHorizontal(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const;
    void isValidMoveVertical(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const;
    void isValidMoveBlock(quint8 x, quint8 y, quint8 val, QList<QObject *> &invalidList) const;

private:
    friend class BoardGenerator;
    friend class Game;

    /**
      * Sets the value of the cell at coordinates x, y to the given value.
      */
    void setCellValue(quint8 x, quint8 y, quint8 value);

    void clearCell(quint8 x, quint8 y) { setCellValue(x, y, 0); }

private:
    friend class Cell;
    friend class GameMessage;
    friend QDataStream &operator<<(QDataStream &stream, Game &game);
    friend QDataStream &readGameV1(QDataStream &stream, Game &game);
    friend QDataStream &readGameV2(QDataStream &stream, Game &game);

    Cell m_cells[9][9];
    quint8 m_cellValues[9][9];

    quint8 m_solution[9][9];

    quint64 m_startTime;
    quint64 m_elapsedTime;
    bool m_paused;

    QStack<ModificationLogEntry> modificationLog;
    bool blockModificationLog;

    Sudoku::Difficulty m_difficulty;

    Cell *m_selectedCell;
};

QML_DECLARE_TYPE(Board)

#endif // BOARD_H
