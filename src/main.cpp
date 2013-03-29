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

#include "applicationfactory.h"

#include "board.h"
#include "game.h"
#include "player.h"
#include "adapters/abstractserver.h"
#include "sudoku.h"
#include "highscore.h"
#include "notemodel.h"

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

static void setupBreakpad() {

    QDesktopServices desktopServices;
    QString dataPath =
            desktopServices.storageLocation(QDesktopServices::DataLocation);
    QString crashDumpPath =
            QDir(dataPath).filePath("dumps");
    if (!QDir(crashDumpPath).exists(crashDumpPath))
        QDir(crashDumpPath).mkpath(crashDumpPath);

    google_breakpad::ExceptionHandler eh(crashDumpPath.toStdString(), NULL, dumpCallback, NULL, true);
}
#endif

static void registerTypes() {
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
    // Settings is not an uncreatable type as it is required in some property declarations.
    qmlRegisterType<Settings>("sudoku", 1, 0, "Settings");
    qmlRegisterUncreatableType<QSortFilterProxyModel>("sudoku", 1, 0, "QSortFilterProxyModel", "...");
    qmlRegisterUncreatableType<Note>("sudoku", 1, 0, "Note", "Returned via NoteModel::get");
    qmlRegisterUncreatableType<NoteModel>("sudoku", 1, 0, "NoteModel", "Accessible from Cell via noteModel property.");
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication::setApplicationName("Sudoku United");
    QApplication::setApplicationVersion("1.2.0");

#ifdef ENABLE_BREAKPAD
    setupBreakpad();
#endif

    registerTypes();

    QCoreApplication *app = ApplicationFactory::createApplication(argc, argv);
    int ret = app->exec();

    // Save the board if there is one
    Sudoku *gameInstance = Sudoku::instance();
    if (gameInstance->game() != NULL && gameInstance->game()->board() != NULL
            && !gameInstance->game()->board()->isFull())
        gameInstance->settings()->setLastGame(gameInstance->game());

    gameInstance->settings()->saveSettings();

    return ret;
}
