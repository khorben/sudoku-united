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

    LoadingOverlay {
        id: loadingOverlay
        text: "Generating board"
    }

    Text {
        id: difficultyText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        text: "Choose the difficulty:"
        font.pixelSize: 30
        font.family: UIConstants.FONT_FAMILY
    }

    Button {
        id: simpleButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: difficultyText.bottom
        anchors.topMargin: 20
        height: 50
        width: 200

        Text{
            text: "Simple"
            anchors.centerIn: parent
            font.pixelSize: 20
            font.family: UIConstants.FONT_FAMILY
        }

        onClicked: {
            gameInstance.createGame(1)
        }
    }

    Button {
        id: easyButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: simpleButton.bottom
        anchors.topMargin: 20
        height: 50
        width: 200

        Text{
            text: "Easy"
            anchors.centerIn: parent
            font.pixelSize: 20
            font.family: UIConstants.FONT_FAMILY
        }

        onClicked: {
            gameInstance.createGame(2)
        }
    }

    Button {
        id: intermediateButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: easyButton.bottom
        anchors.topMargin: 20
        height: 50
        width: 200

        Text{
            text: "Intermediate"
            anchors.centerIn: parent
            font.pixelSize: 20
            font.family: UIConstants.FONT_FAMILY
        }

        onClicked: {
            gameInstance.createGame(3)
        }
    }

    Button {
        id: expertButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: intermediateButton.bottom
        anchors.topMargin: 20
        height: 50
        width: 200

        Text{
            text: "Expert"
            anchors.centerIn: parent
            font.pixelSize: 20
            font.family: UIConstants.FONT_FAMILY
        }

        onClicked: {
            gameInstance.createGame(4)
        }
    }
}
