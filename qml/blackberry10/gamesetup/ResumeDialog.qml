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

import bb.system 1.0

SystemDialog {
    id: resumeDialog

    title: "Resume game"
    body: "Do you want to resume your previous game?"

    onFinished: {
        if (result === SystemUiResult.ConfirmButtonSelection) {
            gameInstance.game = gameInstance.settings.lastGame
        }

        gameInstance.settings.lastGame = null
        gameInstance.settings.saveSettings()
    }
}