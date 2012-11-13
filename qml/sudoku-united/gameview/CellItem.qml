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
import "../UIConstants.js" as UIConstants

Rectangle {
    id: cellItem

    property string startColor: "white"
    property string fontColor: "black"
    property variant cell
    property bool selected: false
    property bool highlighted: false
    property alias markedValue: noteGrid.markedValue
    property bool collision: false

    function animateFull() {
        fullAnimation.start()
    }

    width: 50
    height: 50
    border.width: 1
    border.color: "grey"

    color: startColor

    NoteGrid {
        id: noteGrid
        visible: !!cell && !cell.value
        model: !!cell ? cell.noteModel : undefined
    }

    Text {
        id: currentValue
        property real angle // suppress warnings (QTBUG-22141)
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
        font.pixelSize: 24

        ParallelAnimation {
            id: fullAnimation
            RotationAnimation {
                target: currentValue
                duration: 500
                easing.type: Easing.InOutSine
                direction: RotationAnimation.Clockwise
                to: 360
            }
            SequentialAnimation {
                PropertyAnimation {
                    target: currentValue
                    duration: 250
                    easing.type: Easing.InOutSine
                    property: "scale"
                    to: 1.6
                }
                PropertyAnimation {
                    target: currentValue
                    duration: 250
                    easing.type: Easing.InOutSine
                    property: "scale"
                    to: 1.0
                }
            }
            PropertyAction { target: currentValue; property: "rotation"; value: 0 }
        }
    }

    Rectangle {
        radius: 7
        border.width: 2
        border.color: "blue"
        color: "transparent"
        visible: !!cell && cell.value && markedValue == cell.value
        anchors.fill: parent
        anchors.margins: 10
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
}

