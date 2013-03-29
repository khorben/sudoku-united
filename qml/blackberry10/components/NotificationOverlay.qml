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

Container {
    id: root

    property alias text: label.text

    layout: DockLayout {}

    horizontalAlignment: HorizontalAlignment.Fill

    topPadding: 16
    rightPadding: 16
    leftPadding: 16

    visible: false
    opacity: 0.0

    attachedObjects: [
        Quick.Timer {
            id: fadeOutTimer

            interval: 3000

            onTriggered: root.hide()
        }
    ]

    implicitLayoutAnimationsEnabled: false
    animations: [
        FadeTransition {
            id: fadeIn
            duration: 500
            easingCurve: StockCurve.CubicOut
            fromOpacity: 0.0
            toOpacity: 1.0

            onStarted: visible = true
            onEnded: fadeOutTimer.restart()
        },
        FadeTransition {
            id: fadeOut
            duration: 500
            easingCurve: StockCurve.CubicOut
            fromOpacity: 1.0
            toOpacity: 0.0

            onEnded: visible = false
        }
    ]

    function show() {
        if (fadeOut.isPlaying())
            fadeOut.stop()

        fadeOutTimer.stop()
        fadeIn.play()
    }

    function hide() {
        if (fadeIn.isPlaying())
            fadeIn.stop()

        fadeOut.play()
    }

    Rectangle {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        backgroundColor: Color.create("#DC000000")

        borderWidth: 2
        borderColor: Color.DarkGray

        Container {
            topPadding: 40
            rightPadding: 32
            bottomPadding: 40
            leftPadding: 32

            gestureHandlers: [
                TapHandler {
                    onTapped: hide()
                }
            ]

            Label {
                id: label

                textStyle.color: Color.White

                verticalAlignment: VerticalAlignment.Center

                multiline: true
            }
        }
    }
}