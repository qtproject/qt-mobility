TEMPLATE = lib
DESTDIR = com/nokia/mobility
TARGET = qorganizerqmlplugin

CONFIG += qt plugin
QT += declarative
VERSION = 1.0.0

# CONFIG += mobility
# MOBILITY = organizer versit

DEPENDPATH += .
INCLUDEPATH += . \
    ../../include \
    ../../src/organizer \
    ../../src/versit
QT += declarative
QT += script

# Input
SOURCES += plugin.cpp

OTHER_FILES +=  timescape.qml \
                contents/Button.qml \
                contents/ScrollBar.qml \
                contents/monthview.qml \
                contents/detailview.qml \
                contents/settingsview.qml \
                contents/timelineview.qml \
                contents/detailsview.qml \
                com/nokia/mobility/qmldir

symbian:{
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        SwEvent
}

include(../examples.pri)
