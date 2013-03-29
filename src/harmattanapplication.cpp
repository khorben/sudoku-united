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

#include "harmattanapplication.h"

#include "qmlapplicationviewer.h"

#include "player.h"
#include "sudoku.h"

#include <QCoreApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>

QCoreApplication *HarmattanApplication::create(int &argc, char *argv[])
{
    static HarmattanApplication *app = NULL;

    if (!app) {
        createApplication(argc, argv);
        app = new HarmattanApplication();
    }

    return QCoreApplication::instance();
}

HarmattanApplication::HarmattanApplication()
{
    viewer.reset(QmlApplicationViewer::create());

    // We draw our own background
    viewer->setAttribute(Qt::WA_OpaquePaintEvent);
    viewer->setAttribute(Qt::WA_NoSystemBackground);
    viewer->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    viewer->viewport()->setAttribute(Qt::WA_NoSystemBackground);

    viewer->installEventFilter(Sudoku::instance());

    setContextVariables();
    setupGameView();

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    viewer->setMainQmlFile(QLatin1String("qml/sudoku-united/main.qml"));
    viewer->showExpanded();
}

void HarmattanApplication::setContextVariables()
{
    viewer->rootContext()->setContextProperty("qApplication", QApplication::instance());
    viewer->rootContext()->setContextProperty("gameInstance", Sudoku::instance());
    viewer->rootContext()->setContextProperty("localPlayer", Sudoku::instance()->player());
}

void HarmattanApplication::setupGameView()
{
    QDeclarativeComponent *gameViewComponent =
            new QDeclarativeComponent(viewer->engine(),
                                      QUrl::fromLocalFile(adjustPath("qml/sudoku-united/gameview/GameView.qml")));
    QObject *gameView = gameViewComponent->create(viewer->rootContext());
    viewer->rootContext()->setContextProperty("gameView", gameView);
}

/**
  * Copied from QmlApplicationViewerPrivate as it is private there.
  */
QString HarmattanApplication::adjustPath(const QString &path) {
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
