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

Container {
    id: root

    property bool hovered: false
    property bool longTap: false

    signal tapped()
    signal longTapped()

    property int __hoverStart
    property bool __fired: false

    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill

    attachedObjects: [
        Quick.Timer {
            id: longTapTimer

            interval: 800

            onTriggered: {
                __fired = true
                longTapped()
            }
        }
    ]

    onTouch: {
        if (!enabled) {
            hovered = false
            return
        }

        if (event.touchType == TouchType.Down)
            hovered = true
        else if (event.touchType == TouchType.Cancel)
            hovered = false
        else if (event.touchType == TouchType.Up) {
            hovered = false

            if (!__fired)
                tapped()
        }
    }

    onTouchExit: hovered = enabled && false
    onTouchEnter: hovered = enabled && true

    onHoveredChanged: {
        if (hovered) {
            __hoverStart = new Date().getTime()
            __fired = false

            if (longTap)
                longTapTimer.start()
        } else {
            longTapTimer.stop()
        }
    }
}