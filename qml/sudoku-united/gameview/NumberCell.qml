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
import sudoku 1.0

Rectangle {
    id: numberCell

    property variant numberChooser

    signal selected(int number, bool longPress)

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
        id: mouseArea

        property bool __pressedAndHold: false
        anchors.fill: parent

        onPressAndHold: {
            __pressedAndHold = true
            numberCell.selected(numberCell.number, true)
        }
        onClicked: numberCell.selected(numberCell.number, false)
        onPressed: __pressedAndHold = false
        onReleased: {
            // onClicked is not called if onPressAndHold was called (but ignored)
            // Connecting and disconnecting the pressOnHold signal dependening
            // on the value of longPressAction does not yield the desired
            // results either.
            if (__pressedAndHold
                    && gameInstance.settings.longPressAction === Settings.IgnoreAction)
                numberCell.selected(numberCell.number, false)
            __pressedAndHold = false
        }
    }

    states: [
        State {
            name: ""
            when: !numberChooser.cell.noteModel.get(number - 1).modelMarked

            PropertyChanges { target: numberCell; color: "#00000000" }
        },

        State {
            name: "marked"
            when: numberChooser.cell.noteModel.get(number - 1).modelMarked
            PropertyChanges { target: numberCell; color: "#1381dd" }
        }

    ]
}
