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
import ".."
import "../UIConstants.js" as UIConstants

Page {
    id: gameView
    property Game game : gameInstance.game

    orientationLock: PageOrientation.LockPortrait

    tools: GameViewToolBar {
        game: gameView.game
        onHint: gameView.hint()
        onUndo: gameView.undo()
        onLeave: leaveGameDialog.open()
        onSettings: pageStack.push(Qt.createComponent("../Settings.qml"), {pageStack: gameView.pageStack});
    }

    function hint() {
        var hints = game.generateHint()
        if (hints.length > 0) {
            infoBanner.text = "There are mistakes in the puzzle.\nPlease fix them first."
            infoBanner.show()
            hints.forEach(function (cell) {
                              playBoard.cellAt(cell.x, cell.y).collision = true
                          })
        }
    }

    function undo() {
        game.board.undo()
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
                close()
                gameInstance.leave()
            }
        }
    }

    onGameChanged: {
        if (!game) {
            pageStack.pop();
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            // Reset state of game view - it may be restored from cache
            leaveGameDialog.selectedIndex = 1
        }
    }

    BackgroundItem {
        Image {
            source: "qrc:/logo_grid_200x200.png"
            width: 200; height: 200
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
        }
    }

    GameBoard {
        id: playBoard
        board: game && game.board
    }

    GameTimer {
        board: game && game.board
        visible: gameInstance.settings.showGameTimer
        anchors { right: parent.right; bottom: parent.bottom; margins: 8 }
    }

    NumberChooser {
        gameBoard: playBoard
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 10 }
    }

    Connections {
        target: game && game.board
        onFullChanged: {
            if (!game.board.full)
                return;

            var component = Qt.createComponent("WinningScreen.qml");
            var winningScreen =
                component.createObject(parent, {"board": game.board});
            winningScreen.clicked.connect(function () {
                                              gameInstance.leave()
                                          })
        }
    }

    Connections {
        target: game

        onPlayerJoined: {
            infoBanner.text = player.name + " has joined the game"
            infoBanner.show()
        }

        onPlayerLeft: {
            infoBanner.text = player.name + " has left the game"
            infoBanner.show()
        }
    }

    InfoBanner {
        id: infoBanner
    }
}
