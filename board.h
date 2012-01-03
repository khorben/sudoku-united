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

class QDataStream;
class Player;
class Board;
class Game;

class Cell : public QObject {
    Q_OBJECT
    Q_PROPERTY(quint8 value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Player *valueOwner READ valueOwner WRITE setValueOwner NOTIFY valueOwnerChanged)
    Q_PROPERTY(quint8 x READ x CONSTANT)
    Q_PROPERTY(quint8 y READ y CONSTANT)
public:
    Cell() : m_fixedCell(false), m_valueOwner(NULL) {}
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
signals:
    void beforeValueChanged();
    void valueChanged();
    void valueOwnerChanged();
private:
    friend class SudokuBoard;
    friend class BoardGenerator;
    friend class GameMessage;
    friend QDataStream &operator>>(QDataStream &stream, Game &game);

    inline Board *board() const { return (Board *) parent(); }
    friend class Board;

    quint8 m_x;
    quint8 m_y;
    bool m_fixedCell;
    Player *m_valueOwner;
};

QML_DECLARE_TYPE(Cell)

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

    void pause();

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

    void boardIsFull();

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

    /**
      * Sets the value of the cell at coordinates x, y to the given value.
      *
      * This is a method to speed up setting when generating new boards.
      *
      * NOTE: No cellValueChanged signal will be emitted when a cell value
      * is set using this function.
      */
    void setCellValue(quint8 x, quint8 y, quint8 value);

    void clearCell(quint8 x, quint8 y) { setCellValue(x, y, 0); }

private:
    friend class Cell;
    friend class GameMessage;
    friend QDataStream &operator<<(QDataStream &stream, Game &game);
    friend QDataStream &operator>>(QDataStream &stream, Game &game);

    Cell m_cells[9][9];
    quint8 m_cellValues[9][9];

    quint8 m_solution[9][9];

    quint64 m_startTime;
    quint64 m_elapsedTime;
    bool m_paused;

    QStack<ModificationLogEntry> modificationLog;
    bool blockModificationLog;

    Sudoku::Difficulty m_difficulty;
};

QML_DECLARE_TYPE(Board)

#endif // BOARD_H
