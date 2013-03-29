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

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "paintcomponent.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/Color>
#include <bb/cascades/Layout>
#include <bb/ImageData>

class Rectangle : public PaintComponent {
    Q_OBJECT

    Q_PROPERTY(QVariant borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(quint32 borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QVariant backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
    Rectangle(bb::cascades::Container *parent = 0);

    QVariant borderColor() const;
    void setBorderColor(const QVariant &borderColor);

    quint32 borderWidth() const;
    void setBorderWidth(quint32 borderWidth);

    qreal radius() const;
    void setRadius(qreal radius);

    QVariant backgroundColor() const;
    void setBackgroundColor(const QVariant &backgroundColor);

public:
    static QColor toQColor(const bb::cascades::Color &color);

signals:
    void borderColorChanged();
    void borderWidthChanged();

    void radiusChanged();

    void backgroundColorChanged();

protected:
    QImage paint();

    ImageDataMetadata::Ptr cacheMetadata() const;

private:
    bb::cascades::Color m_borderColor;
    quint32 m_borderWidth;

    qreal m_radius;

    bb::cascades::Color m_backgroundColor;
};

#endif
