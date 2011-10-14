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
#include "highscore.h"

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

/**
  * Copied from QmlApplicationViewerPrivate as it is private there.
  */
QString adjustPath(const QString &path) {
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    QString pathInInstallDir;
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    pathInInstallDir = QString::fromAscii("%1/../%2").arg(applicationDirPath, path);

    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
#endif
    return path;
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication *app = createApplication(argc, argv);

    app->setApplicationName("Sudoku United");
    app->setApplicationVersion("1.0.88");

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

    QmlApplicationViewer *appViewer = QmlApplicationViewer::create();
    QDeclarativeView *viewer = appViewer->actualView();

    qRegisterMetaType<Cell *>();
    qRegisterMetaTypeStreamOperators<QUuid>("QUuid");
    qRegisterMetaType<QUuid>();

    qmlRegisterType<Cell>("sudoku", 1, 0, "Cell");
    qmlRegisterType<Board>("sudoku", 1, 0, "Board");
    qmlRegisterType<Game>("sudoku", 1, 0, "Game");
    qmlRegisterType<Player>("sudoku", 1, 0, "Player");
    qmlRegisterUncreatableType<GameInfo>("sudoku", 1, 0, "GameInfo", "Use value returned by the adapters gameList() function.");
    qmlRegisterUncreatableType<GameInfoModel>("sudoku", 1, 0, "GameInfoModel", "Returned via discovery");
    qmlRegisterUncreatableType<Sudoku>("sudoku", 1, 0, "Sudoku", "Global instance provided via the gameInstance variable.");
    qmlRegisterUncreatableType<Settings>("sudoku", 1, 0, "Settings", "Retrieve via gameInstance.settings");
    qmlRegisterUncreatableType<QSortFilterProxyModel>("sudoku", 1, 0, "QSortFilterProxyModel", "...");

    // We draw our own background
    viewer->setAttribute(Qt::WA_OpaquePaintEvent);
    viewer->setAttribute(Qt::WA_NoSystemBackground);
    viewer->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    viewer->viewport()->setAttribute(Qt::WA_NoSystemBackground);

    viewer->installEventFilter(Sudoku::instance());
    viewer->rootContext()->setContextProperty("qApplication", QApplication::instance());
    viewer->rootContext()->setContextProperty("gameInstance", Sudoku::instance());
    viewer->rootContext()->setContextProperty("localPlayer", Sudoku::instance()->player());

    // Preload the game board
    QDeclarativeComponent *gameViewComponent =
            new QDeclarativeComponent(viewer->engine(),
                                      QUrl::fromLocalFile(adjustPath("qml/sudoku-united/gameview/GameView.qml")));
    QObject *gameView = gameViewComponent->create(viewer->rootContext());
    viewer->rootContext()->setContextProperty("gameView", gameView);

    appViewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    appViewer->setMainQmlFile(QLatin1String("qml/sudoku-united/main.qml"));
    appViewer->showExpanded();

    int ret = app->exec();

    // Save the board if there is one
    Sudoku *gameInstance = Sudoku::instance();
    if (gameInstance->game() != NULL && gameInstance->game()->board() != NULL
            && !gameInstance->game()->board()->isFull())
        gameInstance->settings()->setLastGame(gameInstance->game());

    gameInstance->settings()->saveSettings();

    return ret;
}
