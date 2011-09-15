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

Page {
    id: mainPage
    tools: commonToolbar
    anchors.fill: parent

    Header {
        id: topStatusBar
        text: "Sudoku"
    }

    ButtonRow {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: topStatusBar.bottom
        anchors.topMargin: 30

        Button {
            id: createButton
            text: "Create"
            onClicked: {
                var component = Qt.createComponent("CreateGameView.qml");
                pageStack.push(component);
            }
        }

        Button {
            text: "Join"
            onClicked: {
                var component = Qt.createComponent("JoinView.qml")
                pageStack.push(component, { "gameInfoModel": gameInstance.discoverGames() });
            }
        }

        Button {
            text: "Settings"
            onClicked: {
                var component = Qt.createComponent("Settings.qml")
                pageStack.push(component, {});
            }
        }
    }
}
