# Add more folders to ship with the application, here
folder_01.source = qml/sudoku-united
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

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

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE282D591

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
CONFIG += mobility
QT += dbus
MOBILITY += connectivity systeminfo

CONFIG += link_pkgconfig
PKGCONFIG += TelepathyQt4

contains(MEEGO_EDITION,harmattan) {
CONFIG += qdeclarative-boostable
CONFIG += qt-boostable
PKGCONFIG += qdeclarative-boostable
}

# Add dependency to symbian components
# CONFIG += qtquickcomponents

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    src/board.cpp \
    src/player.cpp \
    src/game.cpp \
    src/message.cpp \
    src/boardgenerator.cpp \
    src/sudoku.cpp \
    src/adapters/serveradapter.cpp \
    src/adapters/abstractserver.cpp \
    src/adapters/tcp/tcpserver.cpp \
    src/adapters/abstractclient.cpp \
    src/adapters/tcp/tcpclient.cpp \
    src/adapters/tcp/tcpgameinfo.cpp \
    src/adapters/gameinfo.cpp \
    src/adapters/telepathy/telepathyclient.cpp \
    src/adapters/telepathy/telepathyserver.cpp \
    src/adapters/telepathy/telepathygameinfo.cpp \
    src/adapters/telepathy/telepathyhandler.cpp \
    src/adapters/bluetooth/bluetoothclient.cpp \
    src/adapters/bluetooth/bluetoothserver.cpp \
    src/adapters/bluetooth/bluetoothgameinfo.cpp \
    src/settings.cpp \
    src/highscore.cpp \
    src/notemodel.cpp \
    src/cell.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

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
    src/message.h \
    src/boardgenerator.h \
    src/sudoku.h \
    src/adapters/serveradapter.h \
    src/adapters/abstractserver.h \
    src/adapters/tcp/tcpserver.h \
    src/adapters/abstractclient.h \
    src/adapters/tcp/tcpclient.h \
    src/adapters/tcp/tcpgameinfo.h \
    src/adapters/gameinfo.h \
    src/adapters/telepathy/telepathyclient.h \
    src/adapters/telepathy/telepathyserver.h \
    src/adapters/telepathy/telepathygameinfo.h \
    src/adapters/telepathy/telepathyhandler.h \
    src/adapters/bluetooth/bluetoothclient.h \
    src/adapters/bluetooth/bluetoothserver.h \
    src/adapters/bluetooth/bluetoothgameinfo.h \
    src/settings.h \
    src/highscore.h \
    src/notemodel.h \
    src/cell.h

RESOURCES += resources.qrc

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


