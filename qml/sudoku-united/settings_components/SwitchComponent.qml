import QtQuick 1.1
import com.nokia.meego 1.0
import "../UIConstants.js" as UIConstants

Item {
    id: root

    property alias text: textLabel.text
    property alias subText: subTextLabel.text
    property alias checked: switchComponent.checked

    anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
    height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

    Column {
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

    Switch {
        id: switchComponent

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }
}
