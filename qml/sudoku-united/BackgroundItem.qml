import QtQuick 1.0
import "./UIConstants.js" as UIConstants

Rectangle{
    anchors.fill: parent
    color: "#666666"
    smooth: true
    gradient: Gradient {
        GradientStop{ position: 0; color: UIConstants.BACKGROUND_COLOR_TOP }
        GradientStop{ position: 1; color: UIConstants.BACKGROUND_COLOR_BOTTOM }
    }
}
