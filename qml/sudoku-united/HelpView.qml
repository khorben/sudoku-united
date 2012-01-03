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
import "UIConstants.js" as UIConstants

Page {
    property bool firstUse : false

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: {
                pageStack.pop();
            }
            visible: !firstUse
        }
        ToolButton {
            text: "Continue"
            anchors.centerIn: parent
            visible: firstUse
            onClicked: {
                gameInstance.settings.showedJoinHelp = true
                gameInstance.settings.saveSettings()
                var component = Qt.resolvedUrl("JoinView.qml")
                pageStack.replace(component)
            }
        }
    }

    orientationLock: PageOrientation.LockPortrait

    Header {
        id: header
        text: "Help"
    }

    Flickable {
        id: helpFlickable
        width: parent.width
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        contentHeight: instructions.height + instructions.anchors.topMargin + instructions.anchors.bottomMargin
        contentWidth: parent.width
        clip: true

        Label {
            id: instructions
            anchors { left: parent.left; right: parent.right; top: parent.top; margins: UIConstants.DEFAULT_MARGIN;}
            wrapMode: Text.WordWrap
            text: "<p>In order to host a multiplayer game the host has to declare the game as public. This can be achieved by clicking the settings button while in-game and selecting \"Make game public\".</p>
                  <p>Sudoku United supports two different technologies to play with other players:</p>
            <h3 style='margin: 10px 0px 0px 0px;'>Bluetooth</h3><hr>
            <ol>
            <li>Make sure that both players have enabled Bluetooth in the Sudoku United settings - it is enabled by default.</li>
            <li>As soon as one player has created the game and is within Bluetooth range (approximately up to 10m) you can join the game via the Join view by selecting the other player's device name. Depending on several factors it may take a while before the other player can be found.</li>
            </ol>
            <h3 style='margin: 30px 0px 0px 0px;'>Google Talk / Jabber</h3><hr>
            <ol>
            <li>Make sure that you have added the player you want to play with to your Google Talk contact list and that your availability is set to Online.</li>
            <li>As soon as any player from your contact list creates a new game his nickname will appear in the Join view and you can join the game.</li>
            </ol>
            "
        }
    }

    ScrollDecorator {
        flickableItem: helpFlickable
    }

}
