import QtQuick 1.0
import sudoku 1.0

Grid {
    property Board board

    property Cell selectedCell

    signal cellClicked(variant cellItem)

    id: playBoard

    columns: 3
    rows:  3
    spacing: 2

    anchors {
        horizontalCenter: parent.horizontalCenter
        top: parent.top
        topMargin: 15
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
            blockItems[i] = block.createObject(playBoard)
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

                object.showNumberChooser.connect(cellClicked)
                object.showNumberChooser.connect(_onCellClicked)

                cellItemList.push(object);
            }
        }

        _cellItems = cellItemList
    }

    function _onCellClicked(cellItem) {
        selectedCell = cellItem.cell
    }
}
