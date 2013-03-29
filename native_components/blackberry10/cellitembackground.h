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

#ifndef CELLITEMBACKGROUND_H
#define CELLITEMBACKGROUND_H

#include "rectangle.h"
#include "cell.h"

class CellItemBackground : public Rectangle
{
    Q_OBJECT

    Q_PROPERTY(Cell * cell READ cell WRITE setCell NOTIFY cellChanged)

public:
    CellItemBackground(bb::cascades::Container *parent = 0);

    Cell *cell() const;
    void setCell(Cell *cell);

signals:
    void cellChanged();

protected:
    QImage paint();

    ImageDataMetadata::Ptr cacheMetadata() const;

private slots:
    void updateNotes(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    typedef QSharedPointer<QImage> ImagePtr;
    ImagePtr fontTiles() const;

private:
    void updateMarkedNotes();

private:
    Cell *m_cell;
    quint16 m_markedNotes;
};

#endif // CELLITEMBACKGROUND_H
