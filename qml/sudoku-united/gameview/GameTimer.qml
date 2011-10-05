import QtQuick 1.1
import com.nokia.meego 1.0
import sudoku 1.0

Rectangle {
    property Board board

    id: gameTimer
    color: "white";
    radius: 10
    width: 96
    height: 40

    Text {

    }

    Label {
        anchors.centerIn: parent
        width: 64
        height: 32
        textFormat: Text.PlainText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        id: gameTimeLabel
        text: "00:00"
    }

    Timer {
        id: gameTimeTimer
        interval: 1000
        repeat: true
        triggeredOnStart: true
        running: !screen.minimized
        onTriggered: {
            if (!board) {
                gameTimeLabel.text = "00:00"
                return;
            }

            var elapsed = board.elapsedTime / 1000
            var minutes = parseInt(elapsed / 60)
            var seconds = parseInt(elapsed % 60)

            if (minutes < 10)
                minutes = "0" + minutes;
            if (seconds < 10)
                seconds = "0" + seconds;

            gameTimeLabel.text = minutes + ":" + seconds
        }
    }
}
