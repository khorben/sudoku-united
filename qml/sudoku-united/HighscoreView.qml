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

import Qt 4.7
import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UIConstants

Page {
    id: highscoreView
    tools: commonToolbar

    orientationLock: PageOrientation.LockPortrait

    property variant highscoreModel
    property int offset: 0
    property int lastDifficulty: 0;

    Header {
        id: header
        text: "Highscore"
        color: UIConstants.HEADER_DEFAULT_BACKGROUND_COLOR
    }

    // The delegate for each section header
    Component {
        id: sectionHeading
        Item {
            width: highscoreList.width
            height: 30
            Label {
                id: sectionText
                anchors.right: parent.right
                anchors.rightMargin: UIConstants.DEFAULT_MARGIN;
                anchors.verticalCenter: parent.verticalCenter
                text: section
                color: "darkgrey"
                platformStyle: LabelStyle {
                    fontFamily: UIConstants.FONT_FAMILY_BOLD
                    fontPixelSize: 20
                }
            }
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: UIConstants.DEFAULT_MARGIN;
                anchors.right: sectionText.left
                anchors.rightMargin: UIConstants.DEFAULT_MARGIN;
                height: 1
                color: "darkgrey"
            }

        }
    }

    Component {
        id: listDelegate
        Item {
            width: highscoreList.width
            height: UIConstants.LIST_ITEM_HEIGHT_DEFAULT
            Image {
                id: placeImage
                anchors.left: parent.left
                anchors.leftMargin: UIConstants.DEFAULT_MARGIN;
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/squirtle.png"
                height: 48
                width: height


                Label {
                    text: sectionIndex
                    anchors.centerIn: parent
                    platformStyle: LabelStyle {
                        fontFamily: UIConstants.FONT_FAMILY_BOLD
                        fontPixelSize: 24
                    }
                    color: "white"
                }
            }
            Label {
                id: timeLabel
                text: {
                    var time = new Date(playTime);
                    Qt.formatTime(time , "mm:ss")
                }
                anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                platformStyle: LabelStyle {
                    fontFamily: UIConstants.FONT_FAMILY_BOLD
                    fontPixelSize: 24
                }
            }
            Image {
                anchors.right: parent.right
                anchors.rightMargin: UIConstants.DEFAULT_MARGIN
                anchors.verticalCenter: parent.verticalCenter
                source: numberOfPlayers > 1 ? "image://theme/icon-m-contacts-group" : "image://theme/icon-m-toolbar-contact"
            }
        }
    }

    ListView {
        id: highscoreList

        width: parent.width
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        spacing: 2
        clip: true

        model: highscoreView.highscoreModel
        delegate: listDelegate
        section.property: "difficultyString"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading
    }

    Item {
        id: noHighscoreOverlay
        width: 400
        height: 90
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        visible: highscoreList.count === 0 ? true : false

        Label {
           anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.right: parent.right
            anchors.rightMargin: 16
            text: "You haven't finished a game yet!"
            wrapMode: Text.WordWrap
            color: "darkgrey"
            horizontalAlignment: Text.AlignHCenter
            platformStyle: LabelStyle{
                fontFamily: UIConstants.FONT_FAMILY_LIGHT
                fontPixelSize: UIConstants.FONT_XLARGE
            }
        }
    }
}
