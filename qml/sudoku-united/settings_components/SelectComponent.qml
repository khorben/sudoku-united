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
