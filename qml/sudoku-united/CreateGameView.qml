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
import "./UIConstants.js" as UIConstants

Page {
    id: createGameView

    tools: commonToolbar

    property Game game : gameInstance.game

    onGameChanged: {
        loadingOverlay.open()
    }

    Connections {
        target: game
        onBoardChanged: {
            loadingOverlay.forceClose()
            var component = Qt.resolvedUrl("GameView.qml")
            pageStack.replace(component)
        }
    }

    BackgroundItem{}

    LoadingOverlay {
        id: loadingOverlay
        text: "Generating board"
    }

    Rectangle{
        id: buttonGroup
        radius: 7
        color: "#99FFFFFF"
        width: 400; height: 680
        anchors.centerIn: parent

        Item{
            id: placeholder
            anchors.top: parent.top
            anchors.topMargin: 50
        }

        CreateButton {
            id: simpleButton
            text: "Simple"
            parentItem: placeholder
            onClicked: {
                gameInstance.createGame(1)
            }
        }

        CreateButton {
            id: easyButton
            text: "Easy"
            parentItem: simpleButton
            onClicked: {
                gameInstance.createGame(2)
            }
        }

        CreateButton {
            id: intermediateButton
            parentItem: easyButton
            text: "Intermediate"
            onClicked: {
                gameInstance.createGame(3)
            }
        }

        CreateButton {
            id: expertButton
            text: "Expert"
            parentItem: intermediateButton
            onClicked: {
                gameInstance.createGame(4)
            }
        }
    }
}
