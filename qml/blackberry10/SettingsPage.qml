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

import bb.cascades 1.0
import QtQuick 1.0 as Quick

import "settings"

Page {
    id: root

    titleBar: TitleBar {
        title : "Settings"
    }

    attachedObjects: [
        Quick.Connections {
            target: navigationPane

            onPopTransitionEnded: {
                if (page === root)
                    gameInstance.settings.saveSettings()
            }
        }
    ]

    Container {
        ListView {
            property variant foo: gameInstance.settings

            dataModel: XmlDataModel {
                source: "settings/model.xml"
            }

            listItemComponents: [
                BluetoothComponent { settings: gameInstance.settings },
                GameTimerComponent { settings: gameInstance.settings },
                HapticFeedbackComponent { settings: gameInstance.settings },
                PlayerNameComponent { settings: gameInstance.settings },
                LongPressComponent { settings: gameInstance.settings },
                QuickAccessComponent { settings: gameInstance.settings }
            ]
        }
    }
}