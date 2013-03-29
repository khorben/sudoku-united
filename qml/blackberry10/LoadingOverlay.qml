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

Container {
    id: root

    property alias text: label.text
    property NavigationPaneProperties properties

    property ActionItem __originalBackButton

    signal showTransitionFinished()
    signal hideTransitionFinished()
    signal cancel()

    background: Color.create("#C8000000")

    layout: DockLayout {}

    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill

    visible: false
    opacity: 0.0

    implicitLayoutAnimationsEnabled: false
    animations: [
        FadeTransition {
            id: fadeIn
            duration: 500
            easingCurve: StockCurve.CubicOut
            fromOpacity: 0.0
            toOpacity: 1.0

            onStarted: visible = true
            onEnded: root.showTransitionFinished()
        },
        FadeTransition {
            id: fadeOut
            duration: 500
            easingCurve: StockCurve.CubicOut
            fromOpacity: 1.0
            toOpacity: 0.0

            onEnded: { visible = false; root.hideTransitionFinished() }
        }
    ]
    attachedObjects: [
        ActionItem {
            id: cancelButton

            title: "Cancel"
            onTriggered: {
                cancel()
            }
        }
    ]

    function show() {
        if (visible) {
            if (fadeIn.state === AnimationState.Ended)
                showTransitionFinished()
            return
        }

        fadeIn.play()

        if (!properties)
            return

        __originalBackButton = properties.backButton
        properties.backButton = cancelButton
    }

    function hide() {
        if (!visible) {
            if (fadeOut.state === AnimationState.Ended)
                hideTransitionFinished()
            return
        }

        fadeOut.play()

        if (__originalBackButton) {
            properties.backButton = __originalBackButton
            __originalBackButton = null
        } else {
            properties.resetBackButton()
        }
    }

    Container {
        layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        ActivityIndicator {
            running: true

            horizontalAlignment: HorizontalAlignment.Center

            preferredWidth: 256
            preferredHeight: 256
        }

        Label {
            id: label

            horizontalAlignment: HorizontalAlignment.Center

            textStyle.color: Color.White
        }
    }

}