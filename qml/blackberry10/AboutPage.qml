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

Page {
    property string license: '<html>This program is free software: you can redistribute it and/or modify ' +
                             'it under the terms of the GNU General Public License as published by ' +
                             'the Free Software Foundation, either version 3 of the License, or ' +
                             '(at your option) any later version.<br /><br />' +

                             'This package is distributed in the hope that it will be useful, ' +
                             'but WITHOUT ANY WARRANTY; without even the implied warranty of ' +
                             'MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ' +
                             'GNU General Public License for more details.<br /><br />' +

                             'You should have received a copy of the GNU General Public License ' +
                             'along with this program. If not, see ' +
                             '<a href="http://www.gnu.org/licenses">http://www.gnu.org/licenses</a><br /><br /></html>'

    ScrollView {
        Container {
            layout: StackLayout { orientation: LayoutOrientation.TopToBottom }

            topPadding: 16
            rightPadding: 16
            leftPadding: 16

            ImageView {
                horizontalAlignment: HorizontalAlignment.Center

                imageSource: "asset:///icon.png"
            }

            Label {
                text: "<html><a href='https://gitorious.org/sudoku-united/'>Sudoku United " + qApplication.applicationVersion + "</a></html>"
                horizontalAlignment: HorizontalAlignment.Center
            }

            Label {
                text: "<html><strong>Authors:</strong> Philip Lorenz, Ferdinand Mayet</html>"
                multiline: true
            }

            Label {
                text: "<html><strong>Application Icon:</strong> Philip Daubmeier</html>"
                multiline: true
            }

            Label {
                text: "<html><strong>Contributors:</strong> mthuurne, J-P Nurmi</html>"
                multiline: true
            }

            Label {
                text: "<html><strong>Icons:</strong> <a href=\"http://fortawesome.github.com/Font-Awesome\">Font Awesome</a> by Dave Gandy</html>"
                multiline: true
            }

            Label {
                text: "Board generator based on work by Stephen Ostermiller and Jacques Bensimon"
                multiline: true
            }

            Label {
                text: "<html><strong>License:</strong></html>"
            }
            Label{
                text: license
                textStyle.fontStyle: FontStyle.Italic
                multiline: true
            }
        }
    }
}