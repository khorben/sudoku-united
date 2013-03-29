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
import com.nokia.meego 1.0
import sudoku 1.0

Grid {
    id: chooser

    property GameBoard gameBoard
    property Cell cell: gameBoard.selectedCell
    property string mode: "normal"
    width: height
    rows:  3
    columns: 3
    spacing: 2
    enabled: !!cell && !cell.isFixedCell()
    function setNumber(cell, number) {
        //find collisions
        var collisions = gameBoard.board.isValidMove(cell.x, cell.y, number);

        if (collisions.length === 0) {
            if (cell.value != number) {
                cell.valueOwner = localPlayer
                cell.value = number
            } else {
                cell.valueOwner = null;
                cell.value = 0;
            }
        } else {
            // Find colliding cells
            collisions.forEach(function (cell) {
                                   gameBoard.cellAt(cell.x, cell.y).collision = true
                               });
        }
    }

    Component.onCompleted: {
        var component = Qt.createComponent("NumberCell.qml");
        for (var i = 1; i <= 9; i++) {
            var object = component.createObject(chooser, { "number": i,
                                                    "cell": function () { return cell; } })
            object.selected.connect(updateValue)
        }
    }

    function updateValue(number, longPress) {
        var mode = chooser.mode;

        if (longPress && gameInstance.settings.longPressAction === Settings.IgnoreAction)
            return;

        if (longPress)
            mode = "note"

        if (mode === "note"){
            cell.noteModel.get(number - 1).modelMarked = !cell.noteModel.get(number - 1).modelMarked
        } else {
            setNumber(cell, number)
        }
    }
    Connections {
        target: gameBoard
        onBoardChanged: {
            for (var i = 0; i < chooser.children.length; ++i)
                chooser.children[i].active = !gameBoard.board || !gameBoard.board.isValueFull(i+1);
        }
    }

    Connections {
        target: gameBoard.board
        onValueIsFull: {
            chooser.children[value-1].active = false;
        }
    }
}
