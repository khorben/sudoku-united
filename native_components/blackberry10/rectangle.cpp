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

#include "rectangle.h"
#include "rectangle_p.h"

#include <bb/cascades/Container>

RectangleMetadata::RectangleMetadata(quint32 width,
                                     quint32 height,
                                     bb::cascades::Color borderColor,
                                     quint32 borderWidth,
                                     qreal radius,
                                     bb::cascades::Color backgroundColor) :
    width(width),
    height(height),
    borderColor(borderColor),
    borderWidth(borderWidth),
    radius(radius),
    backgroundColor(backgroundColor)
{

}

uint RectangleMetadata::hash() const
{
    uint result;

    result = ::qHash(width);
    result += ::qHash(height);
    result += ::qHash(borderWidth);
    result += ::qHash((uchar) radius);

    return result;
}

bool RectangleMetadata::operator ==(const ImageDataMetadata &other) const
{
    try {
        const RectangleMetadata &r2 = dynamic_cast<const RectangleMetadata &>(other);
        return width == r2.width
                && height == r2.height
                && borderColor == r2.borderColor
                && borderWidth == r2.borderWidth
                && radius == r2.radius
                && backgroundColor == r2.backgroundColor;
    } catch(std::bad_cast exp) {
        return false;
    }
}

Rectangle::Rectangle(bb::cascades::Container *parent) :
    PaintComponent(ImageDataCache::instance<RectangleMetadata>(), parent),
    m_borderColor(bb::cascades::Color::Transparent),
    m_borderWidth(0),
    m_radius(0.0),
    m_backgroundColor(bb::cascades::Color::Transparent)
{
}

QVariant Rectangle::borderColor() const
{
    return QVariant::fromValue(m_borderColor);
}

void Rectangle::setBorderColor(const QVariant &borderColor)
{
    bb::cascades::Color color = borderColor.value<bb::cascades::Color>();

    if (color == m_borderColor)
        return;

    m_borderColor = color;
    emit borderColorChanged();

    requestUpdate();
}

quint32 Rectangle::borderWidth() const
{
    return m_borderWidth;
}

void Rectangle::setBorderWidth(quint32 borderWidth)
{
    if (borderWidth == m_borderWidth)
        return;

    m_borderWidth = borderWidth;
    emit borderWidthChanged();

    requestUpdate();
}

qreal Rectangle::radius() const
{
    return m_radius;
}

void Rectangle::setRadius(qreal radius)
{
    if (radius == m_radius)
        return;

    m_radius = radius;
    emit radiusChanged();

    requestUpdate();
}

QVariant Rectangle::backgroundColor() const
{
    return QVariant::fromValue(m_backgroundColor);
}

void Rectangle::setBackgroundColor(const QVariant &backgroundColor)
{
    bb::cascades::Color color = backgroundColor.value<bb::cascades::Color>();

    if (color == m_backgroundColor)
        return;

    m_backgroundColor = color;
    emit backgroundColorChanged();

    requestUpdate();
}

QColor Rectangle::toQColor(const bb::cascades::Color &color)
{
    return QColor(qBound(0, int(color.red() * 255), 255),
                  qBound(0, int(color.green() * 255), 255),
                  qBound(0, int(color.blue() * 255), 255),
                  qBound(0, int(color.alpha() * 255), 255));
}

QImage Rectangle::paint()
{
    float width = layoutFrame().width();
    float height = layoutFrame().height();

    if (!height || !width)
        return QImage();

    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&image);

    painter.setPen(QPen(QBrush(Rectangle::toQColor(m_borderColor)), m_borderWidth));
    painter.setBrush(QBrush(Rectangle::toQColor(m_backgroundColor)));

    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_radius) {
        painter.drawRoundedRect(0, 0, width, height, m_radius, m_radius);
    } else {
        painter.drawRect(0, 0, width, height);
    }

    painter.end();

    return image;
}

ImageDataMetadata::Ptr Rectangle::cacheMetadata() const
{
    float width = layoutFrame().width();
    float height = layoutFrame().height();

    return ImageDataMetadata::Ptr(new RectangleMetadata(width, height, m_borderColor, m_borderWidth, m_radius, m_backgroundColor));
}
