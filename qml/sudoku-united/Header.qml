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
import com.nokia.meego 1.0
import "./UIConstants.js" as UIConstants

Rectangle {
    property string text : ""
    property bool busy : false

    color: UIConstants.HEADER_DEFAULT_BACKGROUND_COLOR

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 64

    Text {
        id: topStatusBarLabel
        text: parent.text
        font.pixelSize: 26
        font.weight: Font.Bold
        font.family: UIConstants.FONT_FAMILY_BOLD
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 16
        anchors.left: parent.left
    }

    BusyIndicator {
        id: topStatusBarBusyIndicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 16
        running: true
        visible: parent.busy
    }
}
