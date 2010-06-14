TEMPLATE = app
TARGET = qmlcontacts
CONFIG += mobility
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
QT += declarative
QT += script

# Input
SOURCES += main.cpp \
    qmlcontactmodel.cpp

HEADERS += qmlcontactmodel.h

RESOURCES += qmlcontacts.qrc
OTHER_FILES += example.qml \
    contents/example.vcf \
    contents/MediaButton.qml \
    ScrollBar.qml
symbian::TARGET.CAPABILITY = ReadUserData \
    WriteUserData
include(../../examples/examples.pri)
