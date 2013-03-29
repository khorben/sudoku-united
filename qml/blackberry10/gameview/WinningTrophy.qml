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
import sudoku 1.0
import "../UIFunctions.js" as UIFunctions

Container {
    property Board board

    layout: DockLayout {}

    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill

    visible: !!board && board.full

    onTouch: {
        if (event.touchType == TouchType.Up)
            gameInstance.leave()
    }

    onVisibleChanged: {
        if (visible)
            label.text = UIFunctions.formatDuration(board ? board.elapsedTime : 0)
    }

    Rectangle {
        id: winningScreen

        preferredWidth: 360
        preferredHeight: 360

        layout: DockLayout {}

        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        radius: 20

        borderWidth: 4
        borderColor: Color.LightGray

        backgroundColor: Color.create("#0057b1")

        ImageView {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            imageSource: "asset:///trophy.png"
        }

        Container {
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center

            bottomPadding: 16

            Label {
                id: label

                textStyle.fontSize: FontSize.XLarge
                textStyle.color: Color.White
            }
        }
    }
}
