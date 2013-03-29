# Enable Qt Mobility

!load(mobilityconfig) {
    error("Qt Mobility is required.")
}

CONFIG += mobility

qml.source = qml/sudoku-united
qml.target = qml
DEPLOYMENTFOLDERS = qml

CONFIG += link_pkgconfig

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

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

SOURCES += src/harmattanapplication.cpp
HEADERS += src/harmattanapplication.h

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

        return(false)
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

DESTDIR = .

# Please do not modify the following two lines. Required for deployment.
include(../qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
