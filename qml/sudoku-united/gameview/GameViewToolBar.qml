import QtQuick 1.1
import com.nokia.meego 1.0
import sudoku 1.0

ToolBarLayout {
    property Game game
    property Board board: game && game.board

    signal leave()
    signal undo()
    signal hint()
    signal settings()

    visible: true

    ToolIcon {
        iconId: "toolbar-back"
        onClicked: leave()
    }

    ToolButton {
        id: undoButton
        text: "Undo"
        visible: true
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: board != null && game.board.canUndo
        onClicked: undo()
    }

    ToolIcon {
        platformIconId: "toolbar-view-menu"
        anchors.right: parent === undefined ? undefined : parent.right
        onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()

        Menu {
            id: myMenu
            visualParent: pageStack
            MenuLayout {
                MenuItem {
                    text: "Settings"
                    onClicked: settings()
                }
                MenuItem {
                    text: "Hint"
                    enabled: game != null && !game.generationRunning
                    onClicked: hint()
                }
            }
        }
    }
}
