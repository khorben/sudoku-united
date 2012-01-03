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

ToolBarLayout {
    property Game game
    property Board board: game && game.board

    signal leave()
    signal undo()
    signal hint()
    signal settings()

    visible: true

    ToolIcon {
        iconId: "toolbar-back"
        onClicked: leave()
    }

    ToolButton {
        id: undoButton
        text: "Undo"
        visible: gameInstance.settings.quickAccessAction == Settings.UndoAction
        anchors.centerIn: parent
        enabled: board != null && game.board.canUndo
        onClicked: undo()
    }

    ToolButton {
        id: hintButton
        text: "Hint"
        visible: gameInstance.settings.quickAccessAction == Settings.HintAction
        anchors.centerIn: parent
        enabled: game != null && !game.generationRunning
        onClicked: hint()
    }

    ToolIcon {
        platformIconId: "toolbar-view-menu"
        anchors.right: parent === undefined ? undefined : parent.right
        onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()

        Menu {
            id: myMenu
            visualParent: pageStack
            MenuLayout {
                id: menuList

                MenuItem {
                    text: "Settings"
                    onClicked: settings()
                }

                MenuItemFixed {
                    visible: game != null
                    text: game ? (game.publicGame ? "Make game private" : "Make game public") : ""
                    onClicked: {
                        game.publicGame = !game.publicGame
                    }
                }

                MenuItemFixed {
                    text: "Hint"
                    visible: gameInstance.settings.quickAccessAction != Settings.HintAction
                    enabled: game != null && !game.generationRunning
                    onClicked: hint()
                }

                MenuItemFixed {
                    id: undoMenuEntry
                    text: "Undo"
                    visible: gameInstance.settings.quickAccessAction != Settings.UndoAction
                    enabled: board != null && game.board.canUndo
                    onClicked: undo()
                }
            }
        }
    }
}
