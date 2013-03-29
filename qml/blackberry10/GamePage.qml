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
import QtQuick 1.0 as Quick
import sudoku 1.0
import Components 1.0

import "gameview"
import "components"

Page {
    id: root

    property Game game: gameInstance.game

    attachedObjects: [
        ActionItem {
            id: hintAction

            enabled: !!root.game && !!root.game.board && !root.game.generationRunning && !root.game.board.full
            title: "Hint"
            imageSource: "asset:///hint.png"

            onTriggered: {
                if (root.game.generateHint().length) {
                    notificationOverlay.text = "There are mistakes in the puzzle. Please fix them first."
                    notificationOverlay.show()
                }
            }
        },
        ActionItem {
            id: undoAction

            enabled: !!root.game && !!root.game.board && root.game.board.canUndo && !root.game.board.full
            title: "Undo"
            imageSource: "asset:///undo.png"

            onTriggered: root.game.board.undo()
        },
        Quick.Connections {
            target: gameInstance.settings
            onQuickAccessActionChanged: __placeQuickAccessButtons()
        },
        Quick.Connections {
            target: gameInstance
            onGameChanged: {
                if (navigationPane.top == root && !gameInstance.game)
                    navigationPane.popAndDestroy()
                else {
                    navigationPane.remove(root)
                    root.destroy()
                }
            }
        },
        Quick.Connections {
            target: navigationPane
            onTopChanged: {
                navigationPane.peekEnabled = !(navigationPane.top === root)

                if (game && game.board) {
                    if (navigationPane.top === root)
                        game.board.unpause()
                    else if (gameInstance.canPause())
                        game.board.pause()

                }
            }
        },
        LeaveDialog {
            id: leaveDialog
        }
    ]

    paneProperties: NavigationPaneProperties {
                        backButton: ActionItem {
                            title: "Leave"

                            onTriggered: leaveDialog.show()
                        }
                    }

    onCreationCompleted: __placeQuickAccessButtons()

    function __placeQuickAccessButtons() {
        // Enforce quick access button placement via function
        // rather than property bindings as a change of placement
        // via bindings results in duplicate buttons

        var hintPlacement = ActionBarPlacement.OnBar
        var undoPlacement = ActionBarPlacement.OnBar

        switch(gameInstance.settings.quickAccessAction) {
        case Settings.HintAction:
            undoPlacement = ActionBarPlacement.InOverflow
            break
        case Settings.UndoAction:
            hintPlacement = ActionBarPlacement.InOverflow
            break
        }

        removeAction(hintAction)
        removeAction(undoAction)
        addAction(hintAction, hintPlacement)
        addAction(undoAction, undoPlacement)
    }

    BackgroundComponent {
        layout: DockLayout {}

        Container {
            layout: StackLayout {}

            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            GameBoard {
                id: gameBoard

                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center

                topPadding: 32

                board: game ? game.board : null
            }

            Container {
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

                horizontalAlignment: HorizontalAlignment.Fill

                topPadding: 32

                Container {
                    layout: DockLayout {}

                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                    verticalAlignment: VerticalAlignment.Bottom

                    GameTimer {
                        id: gameTimer

                        horizontalAlignment: HorizontalAlignment.Center

                        board: game ? game.board : null
                    }
                }

                Container {
                    layout: DockLayout {}

                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                    verticalAlignment: VerticalAlignment.Center

                    NumberChooser {
                        horizontalAlignment: HorizontalAlignment.Center

                        board: game ? game.board : null
                        mode: noteButton.checked ? "note" : ""
                    }
                }

                Container {
                    layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

                    layoutProperties: StackLayoutProperties { spaceQuota: 1.0 }

                    verticalAlignment: VerticalAlignment.Center

                    GameButton {
                        id: noteButton

                        horizontalAlignment: HorizontalAlignment.Center

                        bottomMargin: 16

                        enabled: !!game && !!game.board && !!game.board.selectedCell

                        checkable: true

                        defaultImage: "asset:///note.png"
                        disabledImage: "asset:///note_disabled.png"
                    }

                    GameButton {
                        id: removeButton

                        horizontalAlignment: HorizontalAlignment.Center

                        enabled: !!game && !!game.board && !!game.board.selectedCell
                                    && (game.board.selectedCell.value || (noteButton.checked && !game.board.selectedCell.noteModel.empty))

                        defaultImage: "asset:///remove.png"
                        disabledImage: "asset:///remove_disabled.png"

                        onTriggered: {
                            var selectedCell = game.board.selectedCell
                            if (noteButton.checked)
                                selectedCell.noteModel.clear()
                            else {
                                selectedCell.clear()
                            }
                        }
                    }
                }
            }
        }

        NotificationOverlay {
            id: notificationOverlay
        }

        WinningTrophy {
            board: root.game ? root.game.board : null
        }
    }
}
