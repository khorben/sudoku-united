CONFIG += cascades

LIBS += -lbbsystem -lbbdevice

qml.source = qml/blackberry10
qml.target = qml
DEPLOYMENTFOLDERS = qml

OTHER_FILES += \
    bar-descriptor.xml

SOURCES += \
    native_components/blackberry10/cellitembackground.cpp \
    native_components/blackberry10/datamodeladaptor.cpp \
    native_components/blackberry10/imagedatacache.cpp \
    native_components/blackberry10/paintcomponent.cpp \
    native_components/blackberry10/rectangle.cpp \
    native_components/blackberry10/repeater.cpp \
    native_components/blackberry10/sectiondatamodel.cpp \
    src/bb10application.cpp

HEADERS += \
    native_components/blackberry10/cellitembackground.h \
    native_components/blackberry10/datamodeladaptor.h \
    native_components/blackberry10/imagedatacache.h \
    native_components/blackberry10/paintcomponent.h \
    native_components/blackberry10/rectangle.h \
    native_components/blackberry10/rectangle_p.h \
    native_components/blackberry10/repeater.h \
    native_components/blackberry10/sectiondatamodel.h \
    src/bb10application.h

INCLUDEPATH += src

simulator {
    CONFIG(release, debug|release) {
                DESTDIR = x86/o
    }
    CONFIG(debug, debug|release) {
                DESTDIR = x86/o-g
    }
}

device {
    CONFIG(release, debug|release) {
                DESTDIR = arm/o.le-v7
    }
    CONFIG(debug, debug|release) {
                DESTDIR = arm/o.le-v7-g
    }
}

CONFIG(release, debug|release) {
    TEMPLATE = lib
    QMAKE_CXXFLAGS += -fvisibility=hidden -fPIC
}
