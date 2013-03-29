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
import sudoku 1.0
import QtQuick 1.0 as Quick
import "../components"
import Components 1.0

CellItemBackground {
    id: root

    property int cellIndex: 0
    property int blockIndex: 0
    property Board board

    property alias __cell: root.cell
    property string state: __determineState()

    property variant selectedColor: Color.create("#FFD3CEAA")
    property variant highlightedColor: Color.create("#FFE7E8D1")
    property variant collisionColor: Color.create("#FFF95400")

    // SVG Colors: [ "blue", "orange", "dodgerblue", "peru", "olive", "mediumorchid", "darkkhaki" ]
    property variant playerColors: [ Color.Blue, Color.create("#FFFFA500"), Color.create("#FF1E90FF"), Color.create("#FFCD853F"), Color.create("#FF808000"), Color.create("#FFBA55D3"), Color.create("#FFBDB76B") ]

    layout: DockLayout {}
    cell:  board ? board.cellAt(((blockIndex % 3) * 3) + cellIndex % 3, (Math.floor(blockIndex / 3) * 3) + Math.floor(cellIndex / 3)) : null
    borderWidth: 1
    borderColor: Color.Gray

    backgroundColor: {
        if (state == "")
            return Color.White
        else if (state == "highlighted")
            return root.highlightedColor
        else if (state == "selected")
            return root.selectedColor
        else if (state == "collision")
            return root.collisionColor
    }

    preferredWidth: 80
    preferredHeight: 80

    attachedObjects: [
        Quick.Connections {
            target: root.__cell
            ignoreUnknownSignals: true

            onCollides: __handleCollision()
        },
        Quick.Timer {
            id: collisionFadeOutTimer

            interval: 1000

            onTriggered: state = (function () { return __determineState() })
        }
    ]

    onTouch: {
        if (!root.__cell)
            return

        if (event.touchType !== TouchType.Up)
            return

        if (root.__cell.isFixedCell())
            return

        board.selectedCell = root.__cell
    }

    function __determineCellFontColor() {
        if (!__cell)
            return Color.Black

        if (__cell.isFixedCell())
            return Color.Black

        if (__cell.valueOwner)
            return root.playerColors[__cell.valueOwner.colorIndex]

        return Color.Gray
    }

    function __determineState() {
        if (!__cell || !board.selectedCell)
            return ""

        if (board.selectedCell == __cell)
            return "selected"
        else if (board.selectedCell.x == __cell.x || board.selectedCell.y == __cell.y)
            return "highlighted"
        else
            return ""
    }

    function __handleCollision() {
        state = "collision"
        if (gameInstance.settings.hapticFeedbackEnabled)
            VibrationController.start(75, 100)
        collisionFadeOutTimer.restart()
    }

    Label {
        id: value

        text: root.__cell && root.__cell.value ? root.__cell.value : ""
        textStyle.color: __determineCellFontColor()

        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
    }
}