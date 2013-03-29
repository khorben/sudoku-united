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
import "../UIConstants.js" as UIConstants

Item {
    id: root

    property alias text: textLabel.text
    property alias subText: subTextLabel.text
    property alias dialogTitle: selectionDialog.titleText
    property alias selectedIndex: selectionDialog.selectedIndex
    property alias model: selectionDialog.model

    anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
    height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

    MouseArea {
        anchors.fill: parent
        onClicked: {
            selectionDialog.open()
        }
    }

    SelectionDialog {
        id: selectionDialog
    }

    Row {
        anchors.fill: parent

        Column {
            width: parent.width - comboBoxImage.width
            anchors.verticalCenter: parent.verticalCenter
            Label {
                id: textLabel
                font.weight: Font.Bold
            }

            Label {
                id: subTextLabel
                wrapMode: Text.WordWrap
                platformStyle: LabelStyle {
                    fontPixelSize: UIConstants.FONT_XSMALL
                }
            }
        }

        Image {
            id: comboBoxImage
            width: sourceSize.width
            anchors.verticalCenter: parent.verticalCenter
            source: "image://theme/icon-m-common-combobox-arrow-selected"
        }
    }
}
