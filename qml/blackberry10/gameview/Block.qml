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
import sudoku 1.0
import Components 1.0
import "../components"

Rectangle {
    id: root

    property int blockIndex
    property Board board

    borderWidth: 1
    borderColor: Color.Transparent

    preferredWidth: grid.preferredWidth + 2 * borderWidth
    preferredHeight: grid.preferredHeight + 2 * borderWidth

    Grid {
        id: grid

        columns: 3
        Repeater {
            model: 9
            CellItem {
                board: root.board
                blockIndex: root.blockIndex
                cellIndex: index
            }
        }
    }

}
