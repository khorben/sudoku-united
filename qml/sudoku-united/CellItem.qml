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
    property bool selected: board.selectedCell != undefined && board.selectedCell.x == cell.x && board.selectedCell.y == cell.y
    property bool highlighted: board.selectedCell != undefined && (board.selectedCell.x == cell.x || board.selectedCell.y == cell.y)
    property bool collision: false

    width: 50
    height: 50
    border.width: 1
    border.color: "grey"
    clip: true

    color: startColor

    signal showNumberChooser(variant cell)

    Text {
        id: currentValue
        text: !cell || cell.value === 0 ? "" : cell.value
        color: {
            if (cell && cell.valueOwner) {
                console.log(cell.valueOwner.colorIndex)
                return UIConstants.PLAYER_COLORS[cell.valueOwner.colorIndex]
            } else {
                return fontColor;
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
            showNumberChooser(parent.cell)
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
            PropertyChanges{target: cellItem; fontColor: "grey"}
            PropertyChanges{target: cellItem; startColor: "white"}
        },
        State {
            name: "highlighted"
            when: highlighted && !selected && !cell.isFixedCell()
            PropertyChanges{target: cellItem; startColor: "#FFEFECCA"}
        },
        State {
            name: "highlightedFixed"
            when: highlighted && !selected && cell.isFixedCell()
            PropertyChanges{target: cellItem; fontColor: "grey"}
            PropertyChanges{target: cellItem; startColor: "#FFEFECCA"}
        },
        State {
            name: "selected"
            when: selected
            PropertyChanges{target: cellItem; startColor: "#A39770"}
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
        running: collision && gameInstance.player.feedback
    }
}
