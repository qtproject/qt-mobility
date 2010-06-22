TEMPLATE = lib
DESTDIR = com/nokia/mobility
TARGET = qcontactqmlplugin

CONFIG += qt plugin mobility
QT += declarative

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
HEADERS += qmlcontactmodel.h \
           imageprovider.h \
           qmlcontact.h \
           qmlcontactdetail.h \
           qmlcontactdetailfield.h

SOURCES += plugin.cpp \
    qmlcontactmodel.cpp \
    imageprovider.cpp \
    qmlcontact.cpp \
    qmlcontactdetail.cpp \
    qmlcontactdetailfield.cpp

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

include(../demos.pri)
