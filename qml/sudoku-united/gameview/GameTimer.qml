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
import "../UIFunctions.js" as UIFunctions

Rectangle {
    property Board board

    id: gameTimer
    color: "white";
    radius: 10
    width: 96
    height: 40

    Label {
        anchors.centerIn: parent
        width: 96
        height: 32
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        id: gameTimeLabel
        text: "00:00"
    }

    Timer {
        id: gameTimeTimer
        interval: 1000
        repeat: true
        triggeredOnStart: true
        running: !screen.minimized && gameTimer.visible
        onTriggered: {
            gameTimeLabel.text = UIFunctions.formatDuration(board ? board.elapsedTime : 0)
        }
    }
}
