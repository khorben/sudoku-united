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
    tools: commonToolbar

    orientationLock: PageOrientation.LockPortrait

    property string license: 'This program is free software: you can redistribute it and/or modify ' +
                             'it under the terms of the GNU General Public License as published by ' +
                             'the Free Software Foundation, either version 3 of the License, or ' +
                             '(at your option) any later version.<br /><br />' +

                             'This package is distributed in the hope that it will be useful, ' +
                             'but WITHOUT ANY WARRANTY; without even the implied warranty of ' +
                             'MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ' +
                             'GNU General Public License for more details.<br /><br />' +

                             'You should have received a copy of the GNU General Public License ' +
                             'along with this program. If not, see ' +
                             '<a href="http://www.gnu.org/licenses">http://www.gnu.org/licenses</a><br /><br />'

    LabelStyle{
        id: labelStyle
        textColor: "black"
        fontFamily: UIConstants.FONT_FAMILY_LIGHT
        fontPixelSize: UIConstants.FONT_LSMALL
    }

    Flickable{
        id: flick
        anchors.fill: parent
        anchors.margins: 20

        contentHeight: aboutColumn.height

        Column{
            id: aboutColumn
            width: parent.width
            spacing: 20
            Image{
                source: "qrc:/logo.png"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Label{
                text: "<a href='https://gitorious.org/sudoku-united/'>Sudoku United " + qApplication.applicationVersion + "</a>"
                anchors.horizontalCenter: parent.horizontalCenter
                platformStyle: labelStyle
                onLinkActivated: Qt.openUrlExternally(link)
            }
            Label{
                text: "Copyright © 2011 Philip Lorenz, Ferdinand Mayet, Philip Daubmeier"
                anchors.horizontalCenter: parent.horizontalCenter
                platformStyle: labelStyle
                wrapMode: Text.WordWrap
                width: parent.width
            }
            Label{
                text: "Contributors: mthuurne"
                anchors.horizontalCenter: parent.horizontalCenter
                platformStyle: labelStyle
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Label{
                text: license
                wrapMode: Text.WordWrap
                width: parent.width
                platformStyle: labelStyle
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
    ScrollDecorator {
        flickableItem: flick
        anchors.rightMargin: -UIConstants.DEFAULT_MARGIN
    }
}
