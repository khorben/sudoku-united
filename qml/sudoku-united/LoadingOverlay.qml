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

    /**
      * This signal is emitted after the dialog has been closed.
      */
    signal closed()

    function isVisible() {
        return loadingDialog.status != DialogStatus.Closed
    }

    function close() {
        loadingDialog.status = DialogStatus.Closing
    }

    function forceClose() {
        if (loadingDialog.status == DialogStatus.Opening) {
            _shouldClose = true;
            return;
        }

        loadingDialog.status = DialogStatus.Closed;
    }

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

    property bool _shouldClose : false;
    /**
      * Overwritten to disable click anywhere to close
      */
    function reject() {
    }

    onStatusChanged: {
        if (_shouldClose) {
            _shouldClose = false;
            loadingDialog.status = DialogStatus.Closed;
        } else if (loadingDialog.status == DialogStatus.Closed)
            closed()
    }
}
