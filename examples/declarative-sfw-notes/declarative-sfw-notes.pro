TEMPLATE = app
TARGET = declarative-sfw-notes
INCLUDEPATH += ../../src/serviceframework

include(../examples.pri)

QT += declarative

HEADERS += sfwnotes.h \
           qdeclarativeservice.h
SOURCES += sfwnotes.cpp \
           qdeclarativeservice.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += sfwnotes.qrc

OTHER_FILES = declarative-sfw-notes.qml
