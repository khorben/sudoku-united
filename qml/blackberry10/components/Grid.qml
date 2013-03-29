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

    property int columns

    layout: AbsoluteLayout {}

    onCreationCompleted: __layout()
    onControlAdded: __layout()
    onControlRemoved: __layout()

    function __layout() {
        var index = 0
        var nextX = 0
        var nextY = 0
        var maxY = 0
        var maxX = 0

        for (var i = 0; i < root.controls.length; i++) {
            var control = root.controls[i]

            if (!control.visible)
                continue

            if (!control.layoutProperties)
                control.layoutProperties = Qt.createQmlObject("import bb.cascades 1.0; AbsoluteLayoutProperties {}", control)

            if (!control.preferredWidthSet || !control.preferredHeightSet)
                continue

            var layoutProperties = control.layoutProperties
            layoutProperties.positionX = nextX
            layoutProperties.positionY = nextY

            index++

            maxX = Math.max(nextX + control.preferredWidth, maxX)

            if (index % root.columns == 0) {
                nextX = 0
                nextY += maxY
                maxY = 0
            } else {
                nextX += control.preferredWidth
                maxY = Math.max(maxY , control.preferredHeight)
            }
        }

        root.preferredWidth = maxX
        root.preferredHeight = nextY + maxY
    }
}