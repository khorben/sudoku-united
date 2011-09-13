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
import com.nokia.extras 1.0
import sudoku 1.0

Page {
    id: gameView
    property Game game

    tools: ToolBarLayout {
        visible: true
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: leaveGameDialog.open()
        }
    }

    SelectionDialog {
        id: leaveGameDialog
        titleText: "Leave the game?"
        selectedIndex: 1

        model: ListModel {
            ListElement { name: "Yes" }
            ListElement { name: "No" }
        }
        onSelectedIndexChanged: {
            if (selectedIndex == 0){
                multiplayerAdapter.leave()
            }
        }
    }

    Connections {
        target: multiplayerAdapter

        onPlayerJoined: {
            infoBanner.text = player.name + " has joined the game"
            infoBanner.show()
        }

        onPlayerLeft: {
            infoBanner.text = player.name + " has left the game"
            infoBanner.show()
        }
    }

    onGameChanged: {
        if (!game)
            pageStack.pop();
    }

    Grid {
        columns: 3
        rows:  3
        id: playBoard
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 15

        property variant cellItems

        property Cell selectedCell

        spacing: 2

        Component.onCompleted: {
            var cellItemList = new Array();

            var block = Qt.createComponent("BlockItem.qml")
            var blockItems = []
            for (var i = 0; i < 9; i++){
                blockItems[i] = block.createObject(playBoard)
            }

            var component = Qt.createComponent("CellItem.qml")
            for (var y = 0; y < 9; y++) {
                for (var x = 0; x < 9; x++) {
                    var pos = Math.floor(y /3) * 3 + Math.floor(x / 3)
                    var object = component.createObject(blockItems[pos].gridItem);
                    object.cell = (function (x, y) {
                                       return (function () {
                                                   if (!game || !game.board)
                                                       return null;

                                                   return game.board.cellAt(x, y)
                                               })
                                   })(x, y)
                    object.board = playBoard
                    object.showNumberChooser.connect(onShowNumberChooser)

                    cellItemList.push(object);
                }
            }

            cellItems = cellItemList
        }

        function onShowNumberChooser(cell) {
            numberChooser.visible = true
            numberChooser.cell = cell
            selectedCell = cell
        }
    }

    WinningScreen {
        visible: !game || !game.board ? false : game.board.full

        onClicked: multiplayerAdapter.leave()
    }

    InfoBanner {
        id: infoBanner
    }

    NumberChooser {
        id: numberChooser
        visible: false

        onNumberChosen: {
            //find collisions
            var collisions = game.board.isValidMove(cell.x, cell.y, number);

            if (collisions.length === 0) {
                cell.valueOwner = localPlayer
                cell.value = number
            } else {
                // Find colliding cells
                collisions.forEach(function (cell) {
                                       playBoard.cellItems.forEach(
                                                   function (cellItem) {
                                                       if (cellItem.cell.x == cell.x && cellItem.cell.y == cell.y) {
                                                           cellItem.collision = true
                                                       }
                                                   })
                                   });
            }
        }
    }
}
