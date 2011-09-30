import QtQuick 1.1
import com.nokia.meego 1.0

Label{
    property int value
    property bool marked

    text: marked ? value : ""
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    platformStyle: LabelStyle{
        fontPixelSize: 11
        textColor: "grey"
    }
}
