TEMPLATE = lib
DESTDIR = com/nokia/mobility
TARGET = qcontactqmlplugin

CONFIG += qt plugin mobility
QT += declarative
QT += script

VERSION = 1.0.0

MOBILITY = contacts \
    versit


DEPENDPATH += .
INCLUDEPATH += . \
    ../../include \
    ../../src/contacts \
    ../../src/contacts/requests \
    ../../src/contacts/details \
    ../../src/contacts/filters \
    ../../src/versit

# Input
HEADERS += qmlcontactmodel.h

SOURCES += plugin.cpp \
    qmlcontactmodel.cpp

OTHER_FILES += example.qml \
    contents/example.vcf \
    contents/MediaButton.qml \
    ScrollBar.qml \
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
