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

Rectangle {
    id: root

    property string defaultImage
    property string hoveredImage
    property string checkedImage
    property string disabledImage

    property bool checkable: false
    property bool checked: false

    signal triggered()

    property bool __hovered: false

    layout: DockLayout {}

    backgroundColor: {
        if (!enabled)
            if (!disabledImage)
                return Color.create("#ffc6c6c6")
            else
                return Color.White
        else if (__hovered)
            return Color.create("#ff0a4d85")
        else if (checked)
            return Color.create("#ff1080dd")
        else
            return Color.White
    }

    radius: 25

    preferredWidth: 128
    preferredHeight: 72

    minWidth: preferredWidth
    minHeight: preferredHeight

    maxWidth: preferredWidth
    maxHeight: preferredHeight

    onTouch: {
        if (!enabled) {
            __hovered = false
            return
        }

        if (event.touchType == TouchType.Down)
            __hovered = true
        else if (event.touchType == TouchType.Cancel)
            __hovered = false
        else if (event.touchType == TouchType.Up) {
            __hovered = false

            if (checkable)
                checked = !checked
            else
                triggered()
        }
    }

    onTouchExit: __hovered = enabled && false
    onTouchEnter: __hovered = enabled && true

    ImageView {
        id: icon

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        preferredWidth: 64
        preferredHeight: 64

        imageSource: {
            if (!root.enabled) {
                return root.disabledImage || root.defaultImage
            } else if (root.__hovered) {
                return root.hoveredImage || root.defaultImage
            } else if (root.checked) {
                return root.checkedImage || root.defaultImage
            } else {
                return root.defaultImage
            }
        }

        scalingMethod: ScalingMethod.AspectFit

        touchPropagationMode: TouchPropagationMode.None
    }
}