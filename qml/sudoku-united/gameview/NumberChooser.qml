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

Rectangle{
    property GameBoard gameBoard

    property Cell cell: gameBoard.selectedCell
    property string mode: noteEditMode.checked ? "note" : "normal"

    id: chooser
    width: 200
    height: 260
    color: "#AF222222"
    radius: 7
    enabled: !!cell && !cell.isFixedCell()

    function setNumber(cell, number) {
        //find collisions
        var collisions = gameBoard.board.isValidMove(cell.x, cell.y, number);

        if (collisions.length === 0) {
            if (cell.value != number) {
                cell.valueOwner = localPlayer
                cell.value = number
            }
        } else {
            // Find colliding cells
            collisions.forEach(function (cell) {
                                   gameBoard.cellAt(cell.x, cell.y).collision = true
                               });
        }
    }

    state: "hidden"

    Rectangle {
        id: numberBoard
        width: 180
        height: width

        //property variant cell

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        border.width: 2;
        radius: 0
        border.color: "grey"
        color: enabled ? "white" : "lightgray"

        Grid {
            id: numberGrid
            columns: 3
            rows:  3
            anchors.fill: parent

            Component.onCompleted: {
                var component = Qt.createComponent("NumberCell.qml");
                for (var i = 1; i <= 9; i++) {
                    var object = component.createObject(numberGrid, { "number": i,
                                                            "numberChooser": function () { return chooser; } })
                    object.selected.connect(updateValue)
                }
            }

            function updateValue(number) {
                if ( chooser.mode == "note" ){
                    cell.noteModel.get(number - 1).modelMarked = !cell.noteModel.get(number - 1).modelMarked
                    return;
                } else {
                    setNumber(cell, number)
                }
            }

        }
    }

    ButtonRow{
        anchors.topMargin: 8
        anchors.top: numberBoard.bottom
        anchors.left: numberBoard.left
        width: numberBoard.width
        exclusive: false

        Button{
            iconSource: "image://theme/icon-m-toolbar-delete"
            onClicked: {
                if ( chooser.mode == "note" ){
                    for (var i = 0; i < 9; i++){
                        cell.noteModel.get(i).modelMarked = false
                    }
                } else {
                    cell.value = 0
                }
            }
            checkable: false
        }

        Button{
            iconSource: "image://theme/icon-m-toolbar-edit"
            id: noteEditMode
            checkable: true
        }
    }
}
