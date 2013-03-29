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

# Add more folders to ship with the application, here
qml.source = qml/sudoku-united
qml.target = qml
DEPLOYMENTFOLDERS = qml

# Taken from Butaca pro file
isEmpty(PREFIX) {
    PREFIX = /usr
}
BINDIR = $$PREFIX/bin
DATADIR =$$PREFIX/share

splash.files += splash.jpg
splash.path = $$DATADIR/$${TARGET}/

icon64.path = $$DATADIR/icons/hicolor/64x64/apps
icon64.files += sudoku-united64.png

INSTALLS += splash icon64

# Enable Qt Mobility

!load(mobilityconfig) {
    error("Qt Mobility is required.")
}

!contains(MOBILITY_CONFIG, systeminfo) {
    error("Qt Mobility systeminfo module is required.")
}
CONFIG += mobility
MOBILITY += systeminfo

CONFIG += link_pkgconfig

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
    src/messages/playermessage.cpp

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \

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
    src/messages/playermessage.h

RESOURCES += resources.qrc

# packagesExist was added in Qt 4.8.0
!defined(packagesExist, test) {
    defineTest(packagesExist) {
        # this can't be done in global scope here because qt_functions is loaded
        # before the .pro is parsed, so if the .pro set PKG_CONFIG, we wouldn't know it
        # yet. oops.
        isEmpty(PKG_CONFIG):PKG_CONFIG = pkg-config # keep consistent with link_pkgconfig.prf! too

        for(package, ARGS) {
            !system($$PKG_CONFIG --exists $$package):return(false)
        }

        return(true)
    }
}

# The pkg-config wrapper part of the Harmattan SDK does not support the --exists
# parameter. Hence, everything supported by the SDK is enabled here.
contains(MEEGO_EDITION, harmattan) | packagesExist(TelepathyQt4) {
    OUT = $$system(pkg-config --modversion TelepathyQt4)
    VERSION_MATCH = $$find(OUT, ^0\\.5|6|7|8.*)
    !count(VERSION_MATCH, 1) {
        warning("Unsupported TelepathyQt4 version ($$OUT) supported versions are 0.5 until 0.8.")
    } else {
        !isEqual(ENABLE_TELEPATHY, 0) {
            PKGCONFIG += TelepathyQt4
            SOURCES += src/adapters/telepathy/telepathyclient.cpp \
                       src/adapters/telepathy/telepathyserver.cpp \
                       src/adapters/telepathy/telepathygameinfo.cpp \
                       src/adapters/telepathy/telepathyhandler.cpp
            HEADERS += src/adapters/telepathy/telepathyclient.h \
                       src/adapters/telepathy/telepathyserver.h \
                       src/adapters/telepathy/telepathygameinfo.h \
                       src/adapters/telepathy/telepathyhandler.h
            DEFINES += WITH_TELEPATHY

            QT += dbus

            message("Enabling Telepathy adapter")
        }
    }
} else {
    warning("Disabling Telepathy adapter as TelepathyQt4 was not found.")
}


isEmpty(ENABLE_BLUETOOTH) | !isEqual(ENABLE_BLUETOOTH, 0) {
    !contains(MOBILITY_CONFIG, connectivity) {
        warning("Disabling Bluetooth adapter as Qt Mobility's connectivity module is missing.")
    } else {
        contains(MOBILITY_CONFIG, systeminfo) {
                MOBILITY += systeminfo
                DEFINES += HAVE_SYSTEM_DEVICE_INFO
        }

        MOBILITY += connectivity
        SOURCES += src/adapters/bluetooth/bluetoothclient.cpp \
                   src/adapters/bluetooth/bluetoothserver.cpp \
                   src/adapters/bluetooth/bluetoothgameinfo.cpp
        HEADERS += src/adapters/bluetooth/bluetoothclient.h \
                   src/adapters/bluetooth/bluetoothserver.h \
                   src/adapters/bluetooth/bluetoothgameinfo.h
        DEFINES += WITH_BLUETOOTH
        message("Enabling Bluetooth adapter")
    }
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

contains(MEEGO_EDITION, harmattan) | packagesExist(qdeclarative-boostable) {
    CONFIG += qdeclarative-boostable qt-boostable
    PKGCONFIG += qdeclarative-boostable
    message("Enabling boostable build")
}

!isEmpty(BREAKPAD_PATH) {
    !isEmpty(SCRATCHBOX_PATH) {
        QMAKE_POST_LINK = cp $(TARGET) $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}; $${SCRATCHBOX_PATH}/login $${BREAKPAD_PATH}/src/tools/linux/dump_syms/dump_syms $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}/$(TARGET) > sudoku-united-`date +%Y%m%d%H%M%S`.sym
        BREAKPAD_PATH = $${SCRATCHBOX_PATH}/users/$$(USER)/$${BREAKPAD_PATH}
    } else {
        QMAKE_POST_LINK = $${BREAKPAD_PATH}/src/tools/linux/dump_syms/dump_syms $(TARGET) > sudoku-united-`date +%Y%m%d%H%M%S`.sym
    }

    LIBS += -L$${BREAKPAD_PATH}/src/client/linux -lbreakpad_client
    INCLUDEPATH += $${BREAKPAD_PATH}/src
    DEFINES += ENABLE_BREAKPAD
}

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
