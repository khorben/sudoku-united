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

import "gamesetup"

Page {
    id: root

    attachedObjects: [
        Quick.Connections {
            target: gameInstance

            onGameChanged: {
                if (navigationPane.top !== root)
                    return

                if (gameInstance.game)
                    if (!gameInstance.game.board) {
                        loadingOverlay.text = "Generating board"
                        loadingOverlay.show()
                    } else {
                        __loadGamePage()
                    }
                else
                    loadingOverlay.hide()
            }
        },
        Quick.Connections {
            target: gameInstance.game

            onBoardChanged: {
                if (navigationPane.top !== root)
                    return

                __loadGamePage()
            }
        },
        Quick.Connections {
            target: navigationPane

            onPushTransitionEnded: {
                if (navigationPane.top !== root)
                    return

                if (gameInstance.settings.lastGame)
                    resumeDialog.show()
            }
        },
        ResumeDialog {
            id: resumeDialog
        }
    ]

    paneProperties: NavigationPaneProperties {
                        id: navigationPaneProperties
                        backButton: ActionItem {
                            onTriggered: navigationPane.popAndDestroy()
                        }
                    }

    function __loadGamePage() {
        loadingOverlay.showTransitionFinished.connect(__createGamePage)
        loadingOverlay.text = "Creating board"
        loadingOverlay.show()
        navigationPaneProperties.backButton.enabled = false
    }

    function __createGamePage() {
        loadingOverlay.showTransitionFinished.disconnect(__createGamePage)
        var page = gamePageDefinition.createObject(root.parent)
        navigationPane.pushAndDestroy(page)
    }

    BackgroundComponent {
        layout: DockLayout {}

        Container {
            layout: DockLayout {}

            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill

            topPadding: 64
            rightPadding: 64
            bottomPadding: 64
            leftPadding: 64

            Rectangle {
                layout: DockLayout {}

                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill

                backgroundColor: Color.create("#99FFFFFF")

                radius: 45

                Container {
                    layout: StackLayout {}

                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center

                    DifficultyButton {
                        text: "Simple"
                        difficulty: Sudoku.SIMPLE
                    }

                    DifficultyButton {
                        text: "Easy"
                        difficulty: Sudoku.EASY
                    }

                    DifficultyButton {
                        text: "Intermediate"
                        difficulty: Sudoku.INTERMEDIATE
                    }

                    DifficultyButton {
                        text: "Hard"
                        difficulty: Sudoku.HARD
                    }

                    DifficultyButton {
                        text: "Expert"
                        difficulty: Sudoku.EXPERT
                    }
                }
            }
        }

        LoadingOverlay {
            id: loadingOverlay

            text: "Creating board"
            properties: navigationPaneProperties

            onCancel: gameInstance.game = null
        }
    }
}