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
import com.nokia.meego 1.0
import sudoku 1.0

BorderImage {
    id: numberCell

    property Cell cell

    signal selected(int number, bool longPress)

    property int number
    property bool active: true

    width: parent.width / 3 - parent.spacing / 2
    height: width

    property bool marked: !!cell && !cell.value && cell.noteModel.get(number - 1).modelMarked

    property string mode: !enabled || !active ? "disabled" :
                          mouseArea.pressed && mouseArea.containsMouse ? "pressed" :
                          marked ? "checked" : "normal"

    ButtonStyle {
        id: style
    }

    source: "qrc:/button/" + number + "-" + mode + ".png"
    border.top: number <= 3 ? 16 : 1
    border.left: number % 3 == 1 ? 16 : 1
    border.right: number % 3 == 0 ? 16 : 1
    border.bottom: number >= 7 ? 16 : 1

    onSelected: {
        if (mouseArea.containsMouse && gameInstance.settings.hapticFeedbackEnabled)
            cellEffect.start()
    }

    Text {
        visible: numberCell.active
        text: parent.number
        anchors.centerIn: parent
        font.family: style.fontFamily
        font.weight: style.fontWeight
        font.pixelSize: style.fontPixelSize
        color: mode == "disabled" ? "darkgray" : mode == "pressed" ? style.pressedTextColor :
               mode == "checked" ? style.checkedTextColor : style.textColor
    }

    MouseArea {
        id: mouseArea
        enabled: numberCell.active
        property bool __pressedAndHold: false
        anchors.fill: parent

        onPressAndHold: {
            __pressedAndHold = true
            numberCell.selected(numberCell.number, true)
        }
        onClicked: numberCell.selected(numberCell.number, false)
        onPressed: {
            __pressedAndHold = false

            if (gameInstance.settings.hapticFeedbackEnabled)
                cellEffect.start()
        }
        onReleased: {
            // onClicked is not called if onPressAndHold was called (but ignored)
            // Connecting and disconnecting the pressOnHold signal dependening
            // on the value of longPressAction does not yield the desired
            // results either.
            if (__pressedAndHold
                    && gameInstance.settings.longPressAction === Settings.IgnoreAction)
                numberCell.selected(numberCell.number, false)
            __pressedAndHold = false
        }
    }

    FileEffect {
        id: cellEffect
        source: "/usr/share/themes/base/meegotouch/meego-im-uiserver/feedbacks/priority2_vkb_popup_press/vibra.ivt"
    }
}
