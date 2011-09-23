# Add more folders to ship with the application, here
folder_01.source = qml/sudoku-united
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

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

CONFIG += link_pkgconfig
PKGCONFIG += TelepathyQt4

# Add dependency to symbian components
# CONFIG += qtquickcomponents

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    board.cpp \
    player.cpp \
    game.cpp \
    multiplayeradapter.cpp \
    tcpmultiplayeradapter.cpp \
    message.cpp \
    bluetoothmultiplayeradapter.cpp \
    boardgenerator.cpp \
    sudoku.cpp \
    telepathymultiplayeradapter.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

# Workaround for beta SDK (see http://wiki.meego.com/Porting_Fremantle_Applications_to_Harmattan#Harmattan_scope)
exists($$QMAKE_INCDIR_QT"/../qmsystem2/qmkeys.h"):!contains(MEEGO_EDITION,harmattan): {
  MEEGO_VERSION_MAJOR     = 1
  MEEGO_VERSION_MINOR     = 2
  MEEGO_VERSION_PATCH     = 0
  MEEGO_EDITION           = harmattan
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \

HEADERS += \
    board.h \
    player.h \
    game.h \
    multiplayeradapter.h \
    tcpmultiplayeradapter.h \
    message.h \
    bluetoothmultiplayeradapter.h \
    boardgenerator.h \
    sudoku.h \
    telepathymultiplayeradapter.h

RESOURCES += resources.qrc

QT += dbus
MOBILITY += connectivity

contains(MEEGO_EDITION,harmattan): {
    DEFINES += MEEGO_EDITION_HARMATTAN
}










