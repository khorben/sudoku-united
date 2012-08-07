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

        var block = Qt.createComponent("BlockItem.qml")
        var blockItems = []
        for (var i = 0; i < 9; i++){
            blockItems[i] = block.createObject(grid)
        }

        var component = Qt.createComponent("CellItem.qml")
        for (var y = 0; y < 9; y++) {
            for (var x = 0; x < 9; x++) {
                var pos = Math.floor(y /3) * 3 + Math.floor(x / 3)
                var object = component.createObject(blockItems[pos].gridItem);
                object.cell = (function (x, y) {
                                   return (function () {
                                               if (!board)
                                                   return null;

                                               return board.cellAt(x, y)
                                           })
                               })(x, y)
                object.board = playBoard
                object.collisionChanged.connect(function (cellItem) { return function () { _playHapticFeedback(cellItem) } }(object))

                cellItemList.push(object);
            }
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
