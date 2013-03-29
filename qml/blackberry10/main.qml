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
import "main.js" as Storage

NavigationPane {
    id: navigationPane

    attachedObjects: [
        ComponentDefinition {
            id: settingsPageDefinition
            source: "SettingsPage.qml"
        },
        ComponentDefinition {
            id: gamePageDefinition
            source: "GamePage.qml"
        },
        ComponentDefinition {
            id: gameSetupPageDefinition
            source: "GameSetupPage.qml"
        },
        ComponentDefinition {
            id: aboutPageDefinition
            source: "AboutPage.qml"
        },
        ComponentDefinition {
            id: highscorePageDefinition
            source: "HighscorePage.qml"
        }
    ]

    Menu.definition: MenuDefinition {
        settingsAction: SettingsActionItem {
            onTriggered: {
                var page = settingsPageDefinition.createObject()
                navigationPane.push(page)
            }
        }
        actions: [
            ActionItem {
                title: "About"

                onTriggered: {
                    var page = aboutPageDefinition.createObject()
                    navigationPane.push(page)
                }

                imageSource: "asset:///about.png"
            }
        ]
    }

    function pushAndDestroy(page) {
        Storage.pendingPushRemovals.push({ 'from': navigationPane.top, 'to': page })
        navigationPane.push(page)
    }

    function popAndDestroy() {
        var to = navigationPane.at(navigationPane.count() - 2)
        Storage.pendingPopRemovals.push({ 'from': navigationPane.top, 'to': to })
        navigationPane.pop()
    }

   onPushTransitionEnded: {
       Storage.pendingPushRemovals = Storage.pendingPushRemovals.filter(function(pageInfo) {
           if (pageInfo.to !== navigationPane.top)
               return false

           navigationPane.remove(pageInfo.from)
           pageInfo.from.destroy()
           return true
       })
   }

   onPopTransitionEnded: {
       Storage.pendingPopRemovals = Storage.pendingPopRemovals.filter(function(pageInfo) {
          if (pageInfo.to !== navigationPane.top)
              return false

          pageInfo.from.destroy()
          return true
      })
   }

    MainPage {}
}
