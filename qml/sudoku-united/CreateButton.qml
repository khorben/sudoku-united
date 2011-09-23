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
    height: UIConstants.CREATE_BUTTON_HEIGTH

    Text{
        text: parent.text
        anchors.centerIn: parent
        font.pixelSize: 30
        font.family: UIConstants.FONT_FAMILY
    }
}
