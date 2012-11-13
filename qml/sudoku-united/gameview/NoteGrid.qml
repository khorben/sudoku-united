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

Grid {
    id: grid

    property int markedValue: 0
    property alias model: repeater.model

    columns: 3
    anchors.fill: parent

    Repeater {
        id: repeater

        Text {
            width: grid.width / 3
            height: grid.height / 3

            color: "gray"
            font.pixelSize: 12
            text: modelMarked ? modelValue : ""
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom

            Rectangle {
                radius: 3
                border.width: 1
                border.color: "blue"
                color: "transparent"
                visible: modelMarked && modelValue == markedValue
                anchors.fill: parent
                anchors.margins: 2
            }
        }
    }
}
