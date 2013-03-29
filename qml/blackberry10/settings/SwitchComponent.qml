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

    property string text
    property string subText
    property alias checked: toggleButton.checked

    layout: DockLayout {}

    topPadding: 16
    bottomPadding: 16
    leftPadding: 16
    rightPadding: 16

    preferredWidth: 768

    Container {
        layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

        horizontalAlignment: HorizontalAlignment.Left
        verticalAlignment: VerticalAlignment.Center

        Label {
            textStyle.fontSize: FontSize.Default

            text: root.text
        }

        Label {
            textStyle.fontSize: FontSize.XSmall

            text: root.subText
        }
    }

    Container {
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Center

        ToggleButton {
            id: toggleButton
        }
    }
}