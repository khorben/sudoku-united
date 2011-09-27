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
import "./UIConstants.js" as UIConstants
import com.nokia.meego 1.0

Button {
    property string text
    property variant parentItem

    id: expertButton
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parentItem.bottom
    anchors.topMargin: 20

    width: UIConstants.CREATE_BUTTON_WIDTH
    height: UIConstants.CREATE_BUTTON_HEIGHT

    Text{
        text: parent.text
        anchors.centerIn: parent
        font.pixelSize: 30
        font.family: UIConstants.FONT_FAMILY
    }
}
