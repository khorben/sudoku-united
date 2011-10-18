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
                anchors { top: parent.top; left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }

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

            Item {
                anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
                height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        text: "Bluetooth control"
                        font.weight: Font.Bold

                    }

                    Label {
                        wrapMode: Text.WordWrap
                        platformStyle: LabelStyle {
                            fontPixelSize: UIConstants.FONT_XSMALL
                        }

                        text: "Enable playing via Bluetooth"
                    }
                }

                Switch {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: gameInstance.settings.bluetoothEnabled
                    onCheckedChanged: {
                        gameInstance.settings.bluetoothEnabled = checked
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
                height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        text: "Haptic feedback"
                        font.weight: Font.Bold

                    }

                    Label {
                        wrapMode: Text.WordWrap
                        platformStyle: LabelStyle {
                            fontPixelSize: UIConstants.FONT_XSMALL
                        }

                        text: "Give haptic feedback on special events"
                    }
                }

                Switch {
                    id: feedbackSwitch
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: gameInstance.settings.hapticFeedbackEnabled
                    onCheckedChanged: {
                        gameInstance.settings.hapticFeedbackEnabled = checked
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
                height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Label {
                        text: "Show game timer"
                        font.weight: Font.Bold

                    }

                    Label {
                        wrapMode: Text.WordWrap
                        platformStyle: LabelStyle {
                            fontPixelSize: UIConstants.FONT_XSMALL
                        }

                        text: "Shows the elapsed time in-game"
                    }
                }

                Switch {
                    id: showGameTimerSwitch
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    checked: gameInstance.settings.showGameTimer
                    onCheckedChanged: {
                        gameInstance.settings.showGameTimer = checked
                    }
                }
            }

            Item {
                anchors { left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }
                height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        quickAccessDialog.open()
                    }
                }

                SelectionDialog {
                    id: quickAccessDialog
                    titleText: "Quick access button"

                    model: ListModel {
                        ListElement { name: "Undo" }
                        ListElement { name: "Hint" }
                    }

                    onSelectedIndexChanged: {
                        var action;

                        switch (selectedIndex) {
                        case 0:
                            action = Settings.UndoAction;
                            break;
                        case 1:
                            action = Settings.HintAction;
                            break;
                        }

                        gameInstance.settings.quickAccessAction = action
                    }

                    selectedIndex: gameInstance.settings.quickAccessAction
                }

                Row {
                    width: parent.width
                    Column {
                        width: parent.width - comboBoxImage.width
                        anchors.verticalCenter: parent.verticalCenter
                        Label {
                            text: "Quick access button"
                            font.weight: Font.Bold

                        }

                        Label {
                            wrapMode: Text.WordWrap
                            platformStyle: LabelStyle {
                                fontPixelSize: UIConstants.FONT_XSMALL
                            }

                            text: {
                                switch(gameInstance.settings.quickAccessAction) {
                                case Settings.UndoAction:
                                    return "Undo";
                                case Settings.HintAction:
                                    return "Hint";
                                }
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
        }
    }


    onStatusChanged: {
        if (state == PageStatus.Inactive)
            gameInstance.settings.saveSettings()
    }
}
