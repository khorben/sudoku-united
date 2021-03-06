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

import QtQuick 1.1

Rectangle{

    property alias gridItem: grid
    default property alias data: grid.data

    height: 150
    width: 150
    border.width: 6
    border.color: "#00000000"
    color: "#00000000"

    Grid {
        id: grid
        columns: 3
        rows:  3
        anchors.fill: parent
    }
}
