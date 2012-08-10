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

import QtQuick 1.1
import QtMobility.feedback 1.1
import sudoku 1.0

MouseArea {
    id: playBoard

    property Board board

    property Cell selectedCell

    width: grid.width
    height: grid.height

    Grid {
        id: grid

        columns: 3
        rows:  3
        spacing: 2

        Repeater {
            id: blocks

            model: 9

            BlockItem {
                id: blockItem

                property alias cells: cells
                property int blockIndex: index

                Repeater {
                    id: cells

                    model: 9

                    CellItem {
                        id: cellItem

                        property int row: (blockIndex % 3) * 3 + (index % 3)
                        property int column: Math.floor(blockIndex / 3) * 3 + Math.floor(index / 3)

                        cell: playBoard.board ? playBoard.board.cellAt(row, column) : undefined
                        selected: !!selectedCell && !!cell && selectedCell.x == cell.x && selectedCell.y == cell.y
                        highlighted: !!selectedCell && !!cell && (selectedCell.x == cell.x || selectedCell.y == cell.y)
                        markedValue: !!selectedCell ? selectedCell.value : 0

                        onCollisionChanged: _playHapticFeedback(cellItem)
                    }
                }
            }
        }
    }

    anchors {
        horizontalCenter: parent.horizontalCenter
        top: parent.top
        topMargin: 15
    }

    Connections {
        target: board
        onRowIsFull: {
            for (var i = 0; i < 9; ++i)
                cellAt(i, row).animateFull()
        }
        onColumnIsFull: {
            for (var i = 0; i < 9; ++i)
                cellAt(column, i).animateFull()
        }
        onBlockIsFull: {
            var startX = (block % 3) * 3;
            var startY = Math.floor(block / 3) * 3;
            for (var x = startX; x < startX + 3; ++x) {
                for (var y = startY; y < startY + 3; ++y)
                    cellAt(x, y).animateFull()
            }
        }
        onValueIsFull: {
            for (var i = 0; i < 81; ++i) {
                var cellItem = _cellItems[i]
                if (cellItem.cell.value == value)
                    cellItem.animateFull()
            }
        }
        onBoardIsFull: {
            for (var i = 0; i < 81; ++i)
                _cellItems[i].animateFull()
        }
    }

    function cellAt(x, y) {
        return _cellItems[x % 9 + y * 9];
    }

    property variant _cellItems

    Component.onCompleted: {
        var cellItemList = new Array();

        for (var i = 0; i < blocks.count; ++i) {
            var cells = blocks.itemAt(i).cells;
            for (var j = 0; j < cells.count; ++j)
                cellItemList.push(cells.itemAt(j))
        }

        _cellItems = cellItemList
    }

    onPressed: _updateSelected(mouse)
    onPositionChanged: _updateSelected(mouse)

    function _updateSelected(mouse) {
        var blockItem = grid.childAt(mouse.x,mouse.y);
        if (blockItem && blockItem.gridItem) {
            var pos = blockItem.mapFromItem(grid, mouse.x, mouse.y);
            var cellItem = blockItem.gridItem.childAt(pos.x,pos.y);
            if (cellItem && cellItem.cell)
                selectedCell = cellItem.cell;
        }
    }

    onSelectedCellChanged: cellEffect.start()

    FileEffect {
        id: cellEffect
        source: "/usr/share/themes/base/meegotouch/meego-im-uiserver/feedbacks/priority2_vkb_popup_press/vibra.ivt"
    }

    function _playHapticFeedback(cellItem) {
        if (!cellItem.collision || !gameInstance.settings.hapticFeedbackEnabled)
            return;

        rumbleEffect.start()
    }

    HapticsEffect {
        id: rumbleEffect
        attackIntensity: 0.0
        attackTime: 250
        intensity: 1.0
        duration: 500
        fadeTime: 250
        fadeIntensity: 0.0
    }
}
