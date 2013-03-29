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

#include "cellitembackground.h"
#include "rectangle_p.h"

#include "notemodel.h"

#include <QFontDatabase>

struct CellItemBackgroundMetadata : public RectangleMetadata {
public:
    quint16 markedNotes;

public:
    CellItemBackgroundMetadata(quint32 width = 0,
                      quint32 height = 0,
                      bb::cascades::Color borderColor = bb::cascades::Color::Transparent,
                      quint32 borderWidth = 0,
                      qreal radius = 0.0,
                      bb::cascades::Color backgroundColor = bb::cascades::Color::Transparent,
                      quint16 markedNotes = 0);

    uint hash() const;

    bool operator ==(const ImageDataMetadata &other) const;
};

CellItemBackgroundMetadata::CellItemBackgroundMetadata(quint32 width,
                                     quint32 height,
                                     bb::cascades::Color borderColor,
                                     quint32 borderWidth,
                                     qreal radius,
                                     bb::cascades::Color backgroundColor,
                                     quint16 markedNotes) :
    RectangleMetadata(width, height, borderColor, borderWidth, radius, backgroundColor),
    markedNotes(markedNotes)
{

}

uint CellItemBackgroundMetadata::hash() const
{
    uint result = RectangleMetadata::hash();

    result += ::qHash(markedNotes);

    return result;
}

bool CellItemBackgroundMetadata::operator ==(const ImageDataMetadata &other) const
{
    try {
        const CellItemBackgroundMetadata &r2 = dynamic_cast<const CellItemBackgroundMetadata &>(other);

        return RectangleMetadata::operator ==(other) && markedNotes == r2.markedNotes;
    } catch(std::bad_cast exp) {
    }
    return false;
}

CellItemBackground::CellItemBackground(bb::cascades::Container *parent) :
    Rectangle(parent),
    m_cell(NULL),
    m_markedNotes(0)
{
}

Cell *CellItemBackground::cell() const
{
    return m_cell;
}

void CellItemBackground::setCell(Cell *cell)
{
    if (m_cell == cell)
        return;

    m_cell = cell;
    emit cellChanged();

    if (m_cell) {
        NoteModel *notes = m_cell->noteModel();
        connect(notes, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateNotes(QModelIndex,QModelIndex)));
    }

    updateMarkedNotes();
    requestUpdate();
}

QImage CellItemBackground::paint()
{
    QImage image = Rectangle::paint();

    if (image.isNull() || !m_cell)
        return image;

    QPainter paint(&image);

    const int xSkip = (image.width() - 2 * borderWidth()) / 3;
    const int ySkip = (image.height() - 2 * borderWidth()) / 3;
    const int xStart = borderWidth();

    int x = xStart;
    int y = xStart;

    ImagePtr font = fontTiles();
    const quint16 fontTileWidth = qCeil(font->width() / 9.0);
    const quint16 fontTileHeight = font->height();

    for (int i = 0; i < 9; i++) {
        if ((m_markedNotes & (1 << i))) {
            // QFontDatabase is currently broken so QPainter cannot draw any text directly
            // instead this uses prerendered fonts.
            paint.drawImage(QRect(x + (xSkip - fontTileWidth) / 2, y + (ySkip - fontTileHeight) / 2, fontTileWidth, fontTileHeight), *font, QRect(i * fontTileWidth, 0, fontTileWidth, fontTileHeight));
            // paint.drawText(x, y, xSkip, ySkip, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(i));
        }

        if ((i + 1) % 3 == 0) {
            x = xStart;
            y += ySkip;
        } else {
            x += xSkip;
        }
    }

    paint.end();

    return image;
}

ImageDataMetadata::Ptr CellItemBackground::cacheMetadata() const
{
    float width = layoutFrame().width();
    float height = layoutFrame().height();

    return ImageDataMetadata::Ptr(new CellItemBackgroundMetadata(width, height,borderColor().value<bb::cascades::Color>(), borderWidth(), radius(), backgroundColor().value<bb::cascades::Color>(), m_markedNotes));
}

void CellItemBackground::updateNotes(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)

    updateMarkedNotes();
    requestUpdate();
}

CellItemBackground::ImagePtr CellItemBackground::fontTiles() const
{
    static QWeakPointer<QImage> shared;

    CellItemBackground::ImagePtr result = shared.toStrongRef();

    if (result.isNull()) {
        result = CellItemBackground::ImagePtr(new QImage(QCoreApplication::applicationDirPath() + "/assets/griddigits.png"));
        shared = result.toWeakRef();
    }

    return result;
}

void CellItemBackground::updateMarkedNotes()
{
    if (!m_cell) {
        m_markedNotes = 0;
        return;
    }

    NoteModel *notes = m_cell->noteModel();
    m_markedNotes = 0;
    for (quint8 i = 1; i <= 9; i++) {
        if (notes->isMarked(i))
            m_markedNotes |= 1 << (i - 1);
    }
}
