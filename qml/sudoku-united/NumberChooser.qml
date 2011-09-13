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

Rectangle{
    property variant cell
    id: chooser
    width: 200
    height: 260
    color: "#AF222222"
    radius: 7
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 10

    signal numberChosen(variant cell, int number)

    onVisibleChanged: {
        if(visible){
            state = "active"
        }
        else{
            state = "hidden"
        }
    }

    state: "hidden"

    Rectangle {
        id: numberBoard
        width: 180
        height: width

        //property variant cell

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        border.width: 2;
        radius: 0
        border.color: "grey"


        Grid {
            id: numberGrid
            columns: 3
            rows:  3
            anchors.fill: parent

            Component.onCompleted: {
                var component = Qt.createComponent("NumberCell.qml");
                for (var i = 1; i <= 9; i++) {
                    var object = component.createObject(numberGrid, { "number": i })
                    object.selected.connect(updateValue)
                }
            }

            function updateValue(number) {
                chooser.visible = false;
                numberChosen(cell, number)
            }

        }
    }
    Rectangle{
        width: 60
        height: width
        border.width: 2;
        border.color: "grey"
        anchors.top: numberBoard.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        gradient: Gradient{
            GradientStop{
                color: "white"
                position: 0.0
            }

            GradientStop{
                color: "lightgrey"
                position: 1.0
            }
        }

        Text{
            text: "C"
            anchors.centerIn: parent
            font.pixelSize: 20
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                cell.value = 0
                chooser.visible = false
            }
        }
    }

    states: [State {
            name: "hidden";
            PropertyChanges { target: chooser; height: 0}
            PropertyChanges { target: chooser; width: 0}
            PropertyChanges { target: chooser; visible: false}
        },
        State {
            name: "active";
            PropertyChanges { target: chooser; height: 260}
            PropertyChanges { target: chooser; width: 200}
            PropertyChanges { target: chooser; visible: true}
        }]

    transitions: [
        Transition {
            from: "hidden"; to: "active"; reversible: false
            ParallelAnimation {
                NumberAnimation { properties: "height"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "width"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "visible"; duration: 100; easing.type: Easing.InOutQuad }
            }
        },
        Transition {
            from: "active"; to: "hidden"; reversible: false
            ParallelAnimation {
                NumberAnimation { properties: "height"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "width"; duration: 100; easing.type: Easing.InOutQuad }
                NumberAnimation { properties: "visible"; duration: 100; easing.type: Easing.InOutQuad }
            }
        }
    ]
}
