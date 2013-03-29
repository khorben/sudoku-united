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

import "../UIFunctions.js" as UIFunctions

Rectangle {
    id: root

    property Board board

    layout: DockLayout {}

    preferredWidth: 180
    preferredHeight: 72

    minWidth: preferredWidth
    minHeight: preferredHeight

    maxWidth: preferredWidth
    maxHeight: preferredHeight

    visible: gameInstance.settings.showGameTimer

    backgroundColor: Color.White

    radius: 25

    attachedObjects: [
        Quick.Timer {
            interval: 1000
            repeat: true
            triggeredOnStart: true
            running: root.visible && !!root.board && !root.board.paused
            onTriggered: {
                gameTimer.text = UIFunctions.formatDuration(root.board ? root.board.elapsedTime : 0)
            }
        }
    ]

    Label {
        id: gameTimer

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
    }
}
