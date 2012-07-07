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
import sudoku 1.0
import "settings_components"

Page {
    id: settingsPage

    tools: ToolBarLayout {
        id: commonToolbar
        visible: false
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: {
                pageStack.pop();
            }
            visible: { settingsPage.pageStack.depth<=1 ? false : true }
        }
    }

    orientationLock: PageOrientation.LockPortrait

    Header {
        id: header
        text: "Settings"
    }

    Flickable {
        id: container
        anchors { top: header.bottom; bottom: parent.bottom; left: parent.left; right: parent.right }
        clip: true
        flickableDirection: Flickable.VerticalFlick
        contentHeight: col.height
        contentWidth: col.width
        pressDelay: 100

        Column {
            id: col
            width: container.width
            spacing: UIConstants.DEFAULT_MARGIN

            Column {
                anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }

                /**
                  * Spacing item as the top margin is ignored in the top-most column.
                  */
                Item {
                    anchors { left: parent.left; right: parent.right; }
                    height: UIConstants.DEFAULT_MARGIN
                }

                Label {
                    id: playerNameLabel
                    text: "Player Name"
                    font.weight: Font.Bold
                }

                TextField {
                    text: gameInstance.player.name
                    width: parent.width

                    onTextChanged: {
                        gameInstance.settings.playerName = text
                    }
                }
            }

            SwitchComponent {
                text: "Bluetooth control"
                subText: "Enable playing via Bluetooth"
                checked: gameInstance.settings.bluetoothEnabled
                onCheckedChanged: gameInstance.settings.bluetoothEnabled = checked
            }

            SwitchComponent {
                text: "Haptic feedback"
                subText: "Give haptic feedback on special events"
                checked: gameInstance.settings.hapticFeedbackEnabled
                onCheckedChanged: gameInstance.settings.hapticFeedbackEnabled = checked
            }

            SwitchComponent {
                text: "Show game timer"
                subText: "Shows the elapsed time in-game"
                checked: gameInstance.settings.showGameTimer
                onCheckedChanged: gameInstance.settings.showGameTimer = checked
            }

            SelectComponent {
                text: "Quick access button"
                subText: {
                    switch(gameInstance.settings.quickAccessAction) {
                    case Settings.UndoAction:
                        return "Undo";
                    case Settings.HintAction:
                        return "Hint";
                    }
                }
                dialogTitle: text
                selectedIndex: gameInstance.settings.quickAccessAction
                model: ListModel {
                    ListElement { name: "Undo" }
                    ListElement { name: "Hint" }
                }

                onSelectedIndexChanged: {
                    gameInstance.settings.quickAccessAction = selectedIndex
                }
            }
        }
    }


    onStatusChanged: {
        if (state == PageStatus.Inactive)
            gameInstance.settings.saveSettings()
    }
}
