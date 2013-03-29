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

#include "bb10application.h"

#include <stdio.h>

#include <bb/cascades/Color>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>

#include <bb/system/SystemCredentialsPrompt>
#include <bb/system/SystemDialog>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemToast>
#include <bb/system/SystemUiButton>
#include <bb/system/SystemUiError>
#include <bb/system/SystemUiInputField>
#include <bb/system/SystemUiInputMode>
#include <bb/system/SystemUiPosition>
#include <bb/system/SystemUiResult>

#include <bb/device/VibrationController>

#include "../native_components/blackberry10/repeater.h"
#include "../native_components/blackberry10/rectangle.h"
#include "../native_components/blackberry10/cellitembackground.h"
#include "../native_components/blackberry10/datamodeladaptor.h"
#include "../native_components/blackberry10/sectiondatamodel.h"

#include "game.h"
#include "player.h"
#include "sudoku.h"

using namespace bb::cascades;

void messageHandler(QtMsgType type, const char* msg) {
    Q_UNUSED(type);
    fprintf(stdout, "%s\n", msg);
    fflush(stdout);
}

BB10Application::BB10Application(int &argc, char **argv)
    : Application(argc, argv)
{
    qInstallMsgHandler(messageHandler);

    registerNativeComponents();
    registerSystemUIComponents();

    createCover();
    createRootComponent();

    connect(this, SIGNAL(thumbnail()), SLOT(pauseGame()));
    connect(this, SIGNAL(invisible()), SLOT(pauseGame()));
    connect(this, SIGNAL(asleep()), SLOT(pauseGame()));

    connect(this, SIGNAL(awake()), SLOT(unpauseGame()));
    connect(this, SIGNAL(fullscreen()), SLOT(unpauseGame()));
}

BB10Application *BB10Application::create(int &argc, char *argv[])
{
    static BB10Application *app = NULL;

    if (!app)
        app = new BB10Application(argc, argv);

    return app;
}

void BB10Application::pauseGame()
{
    Sudoku *sudoku = Sudoku::instance();

    if (!sudoku->canPause())
        return;

    sudoku->game()->board()->pause();
}

void BB10Application::unpauseGame()
{
    Sudoku *sudoku = Sudoku::instance();

    if (!sudoku->game() || !sudoku->game()->board())
        return;

    sudoku->game()->board()->unpause();
}

void BB10Application::registerNativeComponents()
{
    qmlRegisterType<Repeater>("Components", 1, 0, "Repeater");
    qmlRegisterType<Rectangle>("Components", 1, 0, "Rectangle");
    qmlRegisterType<CellItemBackground>("Components", 1, 0, "CellItemBackground");
    qmlRegisterType<SectionDataModel>("Components", 1, 0, "SectionDataModel");
    qmlRegisterType<DataModelAdaptor>("Components", 1, 0, "DataModelAdaptor");
    qmlRegisterUncreatableType<QAbstractItemModel>("Components", 1, 0, "QAbstractItemModel", "None");
}

void BB10Application::registerSystemUIComponents()
{
    qmlRegisterType<bb::system::SystemUiButton>("bb.system", 1, 0, "SystemUiButton");
    qmlRegisterType<bb::system::SystemUiInputField>("bb.system", 1, 0, "SystemUiInputField");
    qmlRegisterType<bb::system::SystemToast>("bb.system", 1, 0, "SystemToast");
    qmlRegisterType<bb::system::SystemPrompt>("bb.system", 1, 0, "SystemPrompt");
    qmlRegisterType<bb::system::SystemCredentialsPrompt>("bb.system", 1, 0, "SystemCredentialsPrompt");
    qmlRegisterType<bb::system::SystemDialog>("bb.system", 1, 0, "SystemDialog");
    qmlRegisterUncreatableType<bb::system::SystemUiError>("bb.system", 1, 0, "SystemUiError", "");
    qmlRegisterUncreatableType<bb::system::SystemUiResult>("bb.system", 1, 0, "SystemUiResult", "");
    qmlRegisterUncreatableType<bb::system::SystemUiPosition>("bb.system", 1, 0, "SystemUiPosition", "");
    qmlRegisterUncreatableType<bb::system::SystemUiInputMode>("bb.system", 1, 0, "SystemUiInputMode", "");
    qmlRegisterUncreatableType<bb::system::SystemUiModality>("bb.system", 1, 0, "SystemUiModality", "");
    qRegisterMetaType<bb::system::SystemUiResult::Type>("bb::system::SystemUiResult::Type");
}

void BB10Application::createCover()
{
    SceneCover *cover = new SceneCover(this);

    QmlDocument *qml = QmlDocument::create("asset:///ActiveFrame.qml").parent(this);
    Container *container = qml->createRootObject<Container>();
    cover->setContent(container);

    setCover(cover);
}

void BB10Application::createRootComponent()
{
    QmlDocument *qml = QmlDocument::create("asset:///main.qml");

    if (!qml->hasErrors()) {
        qml->setContextProperty("qApplication", QApplication::instance());
        qml->setContextProperty("gameInstance", Sudoku::instance());
        qml->setContextProperty("localPlayer", Sudoku::instance()->player());
        qml->setContextProperty("VibrationController", new bb::device::VibrationController());

        NavigationPane *appPage = qml->createRootObject<NavigationPane>();

        if (appPage) {
            // Set the main scene for the application to the Page.
            setScene(appPage);
        }
    }
}
