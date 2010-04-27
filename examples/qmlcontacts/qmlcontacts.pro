TEMPLATE = app
TARGET = qmlcontacts

CONFIG += mobility
MOBILITY = contacts versit

DEPENDPATH += .
INCLUDEPATH += . \
               ../../include \
               ../../src/contacts \
               ../../src/contacts/requests \
               ../../src/contacts/details \
               ../../src/contacts/filters \
               ../../src/versit

QT += declarative
QT += script


# Input
SOURCES += main.cpp \
    qmlcontacts.cpp \
    qmlcontact.cpp
HEADERS += qmlcontactsa.h \
    qmlcontact.h
RESOURCES += qmlcontacts.qrc
OTHER_FILES += example.qml \
    contents/example.vcf \
    contents/MediaButton.qml \
    ScrollBar.qml

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        SwEvent
}
include(../examples.pri)
