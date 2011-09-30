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

#include <QtGui/QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"

#include "board.h"
#include "game.h"
#include "player.h"
#include "adapters/abstractserver.h"
#include "sudoku.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlApplicationViewer viewer;

    qRegisterMetaType<Cell *>();

    qmlRegisterType<Cell>("sudoku", 1, 0, "Cell");
    qmlRegisterType<Board>("sudoku", 1, 0, "Board");
    qmlRegisterType<Game>("sudoku", 1, 0, "Game");
    qmlRegisterType<Player>("sudoku", 1, 0, "Player");
    qmlRegisterUncreatableType<GameInfo>("sudoku", 1, 0, "GameInfo", "Use value returned by the adapters gameList() function.");
    qmlRegisterUncreatableType<GameInfoModel>("sudoku", 1, 0, "GameInfoModel", "Returned via discovery");
    qmlRegisterUncreatableType<Sudoku>("sudoku", 1, 0, "Sudoku", "Global instance provided via the gameInstance variable.");
    qmlRegisterUncreatableType<Settings>("sudoku", 1, 0, "Settings", "Retrieve via gameInstance.settings");

    QApplication::instance()->setApplicationName("Sudoku United");
    QApplication::instance()->setApplicationVersion("1.0.0");

    viewer.installEventFilter(Sudoku::instance());
    viewer.rootContext()->setContextProperty("qApplication", QApplication::instance());
    viewer.rootContext()->setContextProperty("gameInstance", Sudoku::instance());
    viewer.rootContext()->setContextProperty("localPlayer", Sudoku::instance()->player());

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/sudoku-united/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
