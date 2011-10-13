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

    property Cell cell
    property variant cellItem
    property string mode: noteEditMode.checked ? "note" : "normal"

    id: chooser
    width: 200
    height: 260
    color: "#AF222222"
    radius: 7

    Connections {
        target: gameBoard

        onCellClicked: {
            chooser.cellItem = cellItem
            chooser.visible = true
            chooser.cell = cellItem.cell
        }
    }

    function setNumber(cell, number) {
        //find collisions
        var collisions = gameBoard.board.isValidMove(cell.x, cell.y, number);

        if (collisions.length === 0) {
            if (cell.value != number) {
                cell.valueOwner = localPlayer
                cell.value = number
            }
            chooser.visible = false;
        } else {
            // Find colliding cells
            collisions.forEach(function (cell) {
                                   gameBoard.cellAt(cell.x, cell.y).collision = true
                               });
        }
    }

    onVisibleChanged: {
        if(visible){
            state = "active"
        }
        else{
            state = "hidden"
            noteEditMode.checked = false
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
                    cellItem.noteModel.get(number - 1).modelMarked = !cellItem.noteModel.get(number - 1).modelMarked
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
            iconSource: "image://theme/icon-m-toolbar-close"
            onClicked: chooser.visible = false
            checkable: false
        }

        Button{
            iconSource: "image://theme/icon-m-toolbar-delete"
            onClicked: {
                if ( chooser.mode == "note" ){
                    for (var i = 0; i < 9; i++){
                        cellItem.noteModel.get(i).modelMarked = false
                    }
                } else {
                    cell.value = 0
                    chooser.visible = false
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

    states: [State {
            name: "hidden";
            PropertyChanges { target: chooser; height: 0}
            PropertyChanges { target: chooser; width: 0}
            PropertyChanges { target: chooser; visible: false}
        },
        State {
            name: "active";
            PropertyChanges { target: chooser; height: 260}
            PropertyChanges { target: chooser; width: 200}
            PropertyChanges { target: chooser; visible: true}
        }]

    transitions: [
        Transition {
            from: "hidden"; to: "active"; reversible: false
            ParallelAnimation {
                NumberAnimation { properties: "height"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "width"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "visible"; duration: 100; easing.type: Easing.InOutQuad }
            }
        },
        Transition {
            from: "active"; to: "hidden"; reversible: false
            ParallelAnimation {
                NumberAnimation { properties: "height"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "width"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "visible"; duration: 100; easing.type: Easing.InOutQuad }
            }
        }
    ]
}
