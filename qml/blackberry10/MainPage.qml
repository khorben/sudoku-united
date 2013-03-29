/**
 * This file is part of Sudoku United.
 *
 * Sudoku United is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sudoku United is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
 */

import bb.cascades 1.0
import Components 1.0
import "gameview"
import "components"

Page {
    id: root

    BackgroundComponent {
        layout: DockLayout {}

        Container {
            layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            ImageView {
                horizontalAlignment: HorizontalAlignment.Center

                preferredHeight: 500
                preferredWidth: 500

                imageSource: "asset:///logo_grid.png"
            }

            Label {
                horizontalAlignment: HorizontalAlignment.Center

                text: "Sudoku United"
                textStyle.color: Color.White
                textStyle.fontSize: FontSize.XXLarge
            }
        }

        Container {
            layout: StackLayout { orientation: LayoutOrientation.LeftToRight }

            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center

            bottomPadding: 128

            Button {
                text: "Play"

                onClicked: {
                    var page = gameSetupPageDefinition.createObject()
                    navigationPane.push(page)
                }
            }

            Button {
                text: "Highscore"

                onClicked: {
                    var page = highscorePageDefinition.createObject()
                    navigationPane.push(page)
                }
            }
        }

        NotificationOverlay {
            text: "There are mistakes in the puzzle"
        }
    }
}
