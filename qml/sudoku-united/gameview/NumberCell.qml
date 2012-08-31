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

BorderImage {
    id: numberCell

    property Cell cell

    signal selected(int number)
    property int number
    property bool active: true

    width: parent.width / 3 - parent.spacing / 2
    height: width

    property bool marked: !!cell && !cell.value && cell.noteModel.get(number - 1).modelMarked

    property string mode: !enabled || !active ? "disabled" :
                          mouseArea.pressed && mouseArea.containsMouse ? "pressed" :
                          marked ? "checked" : "normal"

    source: "qrc:/button/" + number + "-" + mode + ".png"
    border.top: number <= 3 ? 16 : 1
    border.left: number % 3 == 1 ? 16 : 1
    border.right: number % 3 == 0 ? 16 : 1
    border.bottom: number >= 7 ? 16 : 1

    Text {
        visible: numberCell.active
        text: parent.number
        anchors.centerIn: parent
        font.pixelSize: 20
        color: mouseArea.pressed && mouseArea.containsMouse ? "white" : "black"
    }

    MouseArea {
        id: mouseArea
        enabled: numberCell.active
        anchors.fill: parent
        onClicked: {
            numberCell.selected(numberCell.number)
        }
        onPressed: {
            if (gameInstance.settings.hapticFeedbackEnabled)
                cellEffect.start()
        }
        onReleased: {
            if (containsMouse && gameInstance.settings.hapticFeedbackEnabled)
                cellEffect.start()
        }
    }

    FileEffect {
        id: cellEffect
        source: "/usr/share/themes/base/meegotouch/meego-im-uiserver/feedbacks/priority2_vkb_popup_press/vibra.ivt"
    }
}
