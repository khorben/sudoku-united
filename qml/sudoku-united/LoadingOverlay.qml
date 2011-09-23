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
