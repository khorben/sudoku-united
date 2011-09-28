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

import QtQuick 1.0
import QtMobility.feedback 1.1
import "./UIConstants.js" as UIConstants

Rectangle {
    id: cellItem

    property string startColor: "white"
    property string fontColor: "black"
    property variant cell
    property variant board
    property bool selected: board.selectedCell != undefined && cell != undefined && board.selectedCell.x == cell.x && board.selectedCell.y == cell.y
    property bool highlighted: board.selectedCell != undefined && cell != undefined && (board.selectedCell.x == cell.x || board.selectedCell.y == cell.y)
    property bool collision: false

    property ListModel noteModel: ListModel {
        ListElement { modelValue: 1; modelMarked: false }
        ListElement { modelValue: 2; modelMarked: false }
        ListElement { modelValue: 3; modelMarked: false }
        ListElement { modelValue: 4; modelMarked: false }
        ListElement { modelValue: 5; modelMarked: false }
        ListElement { modelValue: 6; modelMarked: false }
        ListElement { modelValue: 7; modelMarked: false }
        ListElement { modelValue: 8; modelMarked: false }
        ListElement { modelValue: 9; modelMarked: false }
    }

    width: 50
    height: 50
    border.width: 1
    border.color: "grey"

    color: startColor

    signal showNumberChooser(variant cell, variant cellItem)

    GridView {
        id: noteGrid
        anchors.fill: parent
        model: noteModel
        cellWidth: (width / 3) - 1
        cellHeight: (height / 3)

        delegate: NoteItem {
            value: modelValue
            marked: modelMarked
            width: GridView.view.cellWidth
            height: GridView.view.cellHeight
        }
    }

    Text {
        id: currentValue
        text: !cell || cell.value === 0 ? "" : cell.value
        color: {
            if (cell && cell.valueOwner) {
                return UIConstants.PLAYER_COLORS[cell.valueOwner.colorIndex]
            } else if (cell && cell.isFixedCell()){
                return fontColor;
            } else {
                // Hint colour
                return "grey";
            }
        }
        anchors.centerIn: parent
        font.pixelSize: 20
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (cell && cell.isFixedCell())
                return;
            showNumberChooser(parent.cell, cellItem)
        }
    }

    state: "default"
    states: [
        State {
            name: "default"
            when: !selected && !highlighted && !collision && !cell.isFixedCell()
            PropertyChanges{target: cellItem; startColor: "white"}
        },
        State {
            name: "collision"
            when: collision
            PropertyChanges{target: cellItem; startColor: "#f95400" }
        },
        State {
            name: "fixed"
            when: cell.isFixedCell() && !highlighted
            PropertyChanges{target: cellItem; fontColor: "black"}
            PropertyChanges{target: cellItem; startColor: "white"}
        },
        State {
            name: "highlighted"
            when: highlighted && !selected && !cell.isFixedCell()
            PropertyChanges{target: cellItem; startColor: "#FFE7E8D1"}
        },
        State {
            name: "highlightedFixed"
            when: highlighted && !selected && cell.isFixedCell()
            PropertyChanges{target: cellItem; fontColor: "black"}
            PropertyChanges{target: cellItem; startColor: "#FFE7E8D1"}
        },
        State {
            name: "selected"
            when: selected
            PropertyChanges{target: cellItem; startColor: "#FFD3CEAA"}
        }
    ]

    transitions: [
        Transition {
            to: "collision"
            SequentialAnimation {
                PauseAnimation{duration: 2000}
                PropertyAction{target: cellItem; property: "collision"; value: false}
            }
        }
    ]

    HapticsEffect {
        id: rumbleEffect
        attackIntensity: 0.0
        attackTime: 250
        intensity: 1.0
        duration: 500
        fadeTime: 250
        fadeIntensity: 0.0
        running: collision && gameInstance.settings.hapticFeedbackEnabled
    }
}

