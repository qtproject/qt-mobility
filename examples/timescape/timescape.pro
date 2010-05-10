TEMPLATE = app
TARGET = timescape

#CONFIG += mobility
#MOBILITY = calendar versit

DEPENDPATH += .
INCLUDEPATH += . \
               ../../include \
               ../../src/calendar \
               ../../src/versit

QT += declarative
QT += script


# Input
SOURCES += main.cpp

RESOURCES += timescape.qrc

OTHER_FILES += contents/application.qml \
    contents/components/Button.qml \
    contents/components/ScrollBar.qml

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        SwEvent
}
include(../examples.pri)
