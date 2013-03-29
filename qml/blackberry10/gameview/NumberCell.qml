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

import bb.cascades 1.0
import Components 1.0
import QtQuick 1.0 as Quick
import sudoku 1.0
import "../components"

Rectangle {
    id: root

    property int number: 1
    property string mode
    property Board board

    property Cell __selectedCell: board && board.selectedCell
    property bool __checked: !!__selectedCell && __selectedCell.noteModel.isMarked(number)
    property variant __checkedColor: Color.create("#1280dc")

    layout: DockLayout {}

    preferredWidth: 100
    preferredHeight: 100

    borderWidth: 1
    backgroundColor: Color.Transparent

    enabled: !!__selectedCell

    attachedObjects: [
        Quick.Connections {
            target: __selectedCell && __selectedCell.noteModel
            onMarkedChanged: {
                if (number != root.number)
                    return

                root.__checked = function () { return !!__selectedCell && __selectedCell.noteModel.isMarked(root.number) }
            }
        }
    ]

    ImageView {
        imageSource: {
            var x = Math.floor((root.number - 1) % 3)
            var y = Math.floor((root.number - 1) / 3)
            var modifier = ""

            if (touchHandler.hovered)
                modifier = "pressed-"
            else
                modifier = ""

            return "asset:///numberselector-" + modifier + x + "-" + y + ".png"
        }

        preferredWidth: root.preferredWidth - root.borderWidth * 2
        preferredHeight: root.preferredHeight - root.borderWidth *2

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        touchPropagationMode: TouchPropagationMode.None
    }

    Label {
        text: root.number

        textStyle.color: {
            if (!root.enabled)
                return Color.Gray

            if (root.__checked)
                return root.__checkedColor

            return Color.Black
        }

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        touchPropagationMode: TouchPropagationMode.None
    }

    TouchHandler {
        id: touchHandler

        longTap: gameInstance.settings.longPressAction == Settings.InsertNote

        onTapped: {
            if (mode != "note") {
                __selectedCell.valueOwner = gameInstance.player
                __selectedCell.value = number
            } else {
                __selectedCell.noteModel.setMarked(root.number, !__selectedCell.noteModel.isMarked(root.number))
            }
        }

        onLongTapped: {
            if (mode != "note") {
                __selectedCell.noteModel.setMarked(root.number, !__selectedCell.noteModel.isMarked(root.number))
            }
        }
    }
}