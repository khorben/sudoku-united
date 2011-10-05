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
import Qt.labs.particles 1.0
import sudoku 1.0

MouseArea {
    property variant playBoard
    anchors.fill: parent

    Rectangle {
        id: winningScreen

        width: 150
        height: 150
        anchors.centerIn: parent
        radius: 20

        border.width: 4
        border.color: "lightgrey"

        smooth: true

        gradient: Gradient{
            GradientStop { color: "#0080cb"; position: 0 }
            GradientStop { color: "#0057b1"; position: 1 }
        }

        Image {
            anchors.centerIn: winningScreen
            source: "qrc:/trophy.png"
        }

        Particles {
            anchors.top: winningScreen.top
            anchors.topMargin: 30
            anchors.horizontalCenter: winningScreen.horizontalCenter
            width: 1
            height: 1
            source: "qrc:/star.png"
            lifeSpan: 2000
            // The particles effect seems to be rendered even if the parent is
            // not visible => force particle count to 0 if not visible
            count: parent.parent.visible ? 200 : 0
            angle: 270
            angleDeviation: 45
            velocity: 80
            velocityDeviation: 30
            ParticleMotionGravity {
                yattractor: 1000
                xattractor: 0
                acceleration: 100
            }
        }

    }
}
