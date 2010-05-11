TEMPLATE = app
TARGET = qmlcalendar

# CONFIG += mobility
# MOBILITY = calendar versit
DEPENDPATH += .
INCLUDEPATH += . \
    ../../include \
    ../../src/calendar \
    ../../src/versit
QT += declarative
QT += script

# Input
SOURCES += main.cpp
RESOURCES += qmlcalendar.qrc
OTHER_FILES += contents/application.qml \
    contents/components/Button.qml \
    contents/components/ScrollBar.qml \
    contents/monthview.qml \
    contents/detailview.qml \
    contents/settingsview.qml \
    contents/timelineview.qml \
    contents/detailsview.qml
symbian::TARGET.CAPABILITY = ReadUserData \
    WriteUserData \
    ReadDeviceData \
    WriteDeviceData \
    SwEvent
include(../examples.pri)
