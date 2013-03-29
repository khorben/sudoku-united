#
# Sudoku United project file
#
# Configuration options for qmake:
# ENABLE_TCP_SERVER=1 : Enables the TCP adapter
# ENABLE_BLUETOOTH=1 (default): Enables the Bluetooth adapter
# ENABLE_TELEPATHY=1 (default): Enables the Telepathy adapter
#
# Setting any of these to 0 disables the respective adapters. Some adapters
# may not be added if development headers are missing.
#

QT += declarative opengl script xmlpatterns sql network svg

contains(DEFINES, Q_OS_BLACKBERRY) {
    include(pri/blackberry10.pri)
}
contains(MEEGO_EDITION, harmattan) {
    include(pri/harmattan.pri)
}

SOURCES += src/main.cpp \
    src/board.cpp \
    src/player.cpp \
    src/game.cpp \
    src/boardgenerator.cpp \
    src/sudoku.cpp \
    src/adapters/serveradapter.cpp \
    src/adapters/abstractserver.cpp \
    src/adapters/abstractclient.cpp \
    src/adapters/gameinfo.cpp \
    src/settings.cpp \
    src/highscore.cpp \
    src/notemodel.cpp \
    src/cell.cpp \
    src/messages/message.cpp \
    src/messages/joinmessage.cpp \
    src/messages/setvaluemessage.cpp \
    src/messages/gamemessage.cpp \
    src/messages/hellomessage.cpp \
    src/messages/playermessage.cpp \
    src/applicationfactory.cpp

HEADERS += \
    src/board.h \
    src/player.h \
    src/game.h \
    src/boardgenerator.h \
    src/sudoku.h \
    src/adapters/serveradapter.h \
    src/adapters/abstractserver.h \
    src/adapters/abstractclient.h \
    src/adapters/gameinfo.h \
    src/settings.h \
    src/highscore.h \
    src/notemodel.h \
    src/cell.h \
    src/messages/message.h \
    src/messages/joinmessage.h \
    src/messages/setvaluemessage.h \
    src/messages/gamemessage.h \
    src/messages/hellomessage.h \
    src/messages/playermessage.h \
    src/applicationfactory.h
        contains(MOBILITY_CONFIG, systeminfo) {
                MOBILITY += systeminfo
                DEFINES += HAVE_SYSTEM_DEVICE_INFO
        }


isEqual(ENABLE_TCP_SERVER, 1) {
    SOURCES += src/adapters/tcp/tcpserver.cpp \
               src/adapters/tcp/tcpclient.cpp \
               src/adapters/tcp/tcpgameinfo.cpp
    HEADERS += src/adapters/tcp/tcpserver.h \
               src/adapters/tcp/tcpclient.h \
               src/adapters/tcp/tcpgameinfo.h
    DEFINES += WITH_TCP_SERVER

    message("Enabling TCP adapter")
}

OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui
