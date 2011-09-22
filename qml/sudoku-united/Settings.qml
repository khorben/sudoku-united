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

import QtQuick 1.0
import com.nokia.meego 1.0
import "UIConstants.js" as UIConstants

Page {
    tools: commonToolbar

    Header {
        id: header
        text: "Settings"
    }

    Column {
        spacing: UIConstants.DEFAULT_MARGIN
        anchors { top: header.bottom; bottom: parent.bottom; left: parent.left; right: parent.right; margins: UIConstants.DEFAULT_MARGIN }

        Column {
            width: parent.width
            Label {
                id: playerNameLabel
                text: "Player Name"
            }

            TextField {
                text: gameInstance.player.name
                width: parent.width

                onTextChanged: {
                    gameInstance.player.name = text
                }
            }
        }

        Item {
            width: parent.width
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

                    text: "Disable Bluetooth when it is not required"
                }
            }

            Switch {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item {
            width: parent.width
            height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT

            Column {
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    text: "Feedback control"
                    font.weight: Font.Bold

                }

                Label {
                    wrapMode: Text.WordWrap
                    platformStyle: LabelStyle {
                        fontPixelSize: UIConstants.FONT_XSMALL
                    }

                    text: "Disable feedback on collisions"
                }
            }

            Switch {
                id: feedbackSwitch
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                checked: gameInstance.player.feedback
                onCheckedChanged: {
                    gameInstance.player.feedback = feedbackSwitch.checked
                }
            }
        }
    }


}
