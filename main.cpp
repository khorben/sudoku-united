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

#ifdef ENABLE_BREAKPAD
#include <client/linux/handler/exception_handler.h>

static bool dumpCallback(const char* dump_path,
                         const char* minidump_id,
                         void* context,
                         bool succeeded)
{
    qWarning() << "Crash dumped saved in" << dump_path << "/" << minidump_id;
    return succeeded;
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::instance()->setApplicationName("Sudoku United");
    QApplication::instance()->setApplicationVersion("1.0.88");

#ifdef ENABLE_BREAKPAD
    QDesktopServices desktopServices;
    QString dataPath =
            desktopServices.storageLocation(QDesktopServices::DataLocation);
    QString crashDumpPath =
            QDir(dataPath).filePath("dumps");
    if (!QDir(crashDumpPath).exists(crashDumpPath))
        QDir(crashDumpPath).mkpath(crashDumpPath);

    google_breakpad::ExceptionHandler eh(crashDumpPath.toStdString(), NULL, dumpCallback, NULL, true);
#endif

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

    // We draw our own background
    viewer.setAttribute(Qt::WA_OpaquePaintEvent);
    viewer.setAttribute(Qt::WA_NoSystemBackground);
    viewer.viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    viewer.viewport()->setAttribute(Qt::WA_NoSystemBackground);

    viewer.installEventFilter(Sudoku::instance());
    viewer.rootContext()->setContextProperty("qApplication", QApplication::instance());
    viewer.rootContext()->setContextProperty("gameInstance", Sudoku::instance());
    viewer.rootContext()->setContextProperty("localPlayer", Sudoku::instance()->player());

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/sudoku-united/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
