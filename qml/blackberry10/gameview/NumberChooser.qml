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
import "../components"
import Components 1.0
import sudoku 1.0

Container {
    id: root

    property Board board

    property string mode

    topMargin: 32

    preferredWidth: 300
    preferredHeight: 300

    minWidth: preferredWidth
    minHeight: preferredHeight

    maxWidth: preferredWidth
    maxHeight: preferredHeight

    layout: DockLayout {}

    Grid {
        columns: 3

        Repeater {
            model: 9
            NumberCell {
                number: (index + 1)
                mode: root.mode
                board: root.board
            }
        }
    }
}