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
import Components 1.0
import sudoku 1.0
import "UIFunctions.js" as UIFunctions

Page {
    id: root

    titleBar: TitleBar {
                  title: "Highscore"
    }

    actions: [
        DeleteActionItem {
            title: "Clear"

            enabled: !gameInstance.highscore.sourceModel.empty

            onTriggered: gameInstance.highscore.sourceModel.clear()
        }
    ]

    Container {
        layout: DockLayout {}

        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        ListView {
            visible: !gameInstance.highscore.sourceModel.empty

            dataModel: DataModelAdaptor {
                model: SectionDataModel {
                    sectionKey: "difficultyString"
                    model: gameInstance.highscore
                }
            }
            listItemComponents: [
                ListItemComponent {
                    type: "header"

                    Header {
                        id: itemHeader

                        title: ListItemData.difficultyString
                    }
                },
                ListItemComponent {
                    type: "item"

                    StandardListItem {
                        id: itemRoot

                        imageSource: "asset:///singleplayer.png"

                        title: ListItemData.playerNames
                        status: UIFunctions.formatDuration(ListItemData.playTime)
                        description: Qt.formatDateTime(ListItemData.finishedDate, Qt.DefaultLocaleShortDate)
                    }
                }
            ]

            function itemType(data, indexPath) {
                return (indexPath.length == 1 ? 'header' : 'item');
            }
        }
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            visible: gameInstance.highscore.sourceModel.empty

            text: "Finish at least one game to see your results here."

            multiline: true
            textStyle.textAlign: TextAlign.Center
        }
    }
}
