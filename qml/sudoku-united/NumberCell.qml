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

Rectangle {
    id: numberCell

    property variant numberChooser

    signal selected(int number)
    property int number

    width: parent.width / 3
    height: width

    border.width: 1
    border.color: "grey"
    color: "#00000000"

    Text {
        text: parent.number
        anchors.centerIn: parent
        font.pixelSize: 20
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            numberCell.selected(numberCell.number)
        }
    }

    states: [
        State {
            name: ""
            when: !numberChooser.cellItem.noteModel.get(number - 1).modelMarked

            PropertyChanges { target: numberCell; color: "#00000000" }
        },

        State {
            name: "marked"
            when: numberChooser.cellItem.noteModel.get(number - 1).modelMarked
            PropertyChanges { target: numberCell; color: "#1381dd" }
        }

    ]
}
