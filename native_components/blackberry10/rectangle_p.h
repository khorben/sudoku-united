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

#ifndef RECTANGLE_P_H
#define RECTANGLE_P_H

struct RectangleMetadata : public ImageDataMetadata {
public:
    quint32 width;
    quint32 height;

    bb::cascades::Color borderColor;
    quint32 borderWidth;

    qreal radius;

    bb::cascades::Color backgroundColor;

public:
    RectangleMetadata(quint32 width = 0,
                      quint32 height = 0,
                      bb::cascades::Color borderColor = bb::cascades::Color::Transparent,
                      quint32 borderWidth = 0,
                      qreal radius = 0.0,
                      bb::cascades::Color backgroundColor = bb::cascades::Color::Transparent);

    uint hash() const;

    bool operator ==(const ImageDataMetadata &other) const;
};

#endif // RECTANGLE_P_H
