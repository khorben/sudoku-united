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

Dialog {
    id: loadingDialog

    /**
      * The text to show to the user while loading is in progress.
      */
    property string text: "Loading"

    /**
      * Shows a cancel button enabling the user to cancel the action.
      */
    property bool showCancelButton: false

    content: Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 16

        BusyIndicator {
            id: busyIndicator
            running: true
            style: BusyIndicatorStyle {
                size: "large"
                inverted: true
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            text: loadingDialog.text
            style: LabelStyle {
                inverted: true
            }
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            visible: loadingDialog.showCancelButton
            text: "Cancel"
            style: ButtonStyle {
                inverted: true
            }
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: rejected()
        }
    }

    /**
      * Overwritten to disable click anywhere to close
      */
    function reject() {
    }

    function close() {
        loadingOverlay.status = DialogStatus.Closing
    }

    function forceClose() {
        loadingOverlay.status = DialogStatus.Closed
    }
}
