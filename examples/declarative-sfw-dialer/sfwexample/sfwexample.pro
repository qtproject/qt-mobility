TEMPLATE = app
TARGET = declarative-sfw-dialer
INCLUDEPATH += ../../../src/serviceframework

include(../../examples.pri)

QT += gui declarative

HEADERS += sfwexample.h \
           qdeclarativeservice.h
SOURCES += sfwexample.cpp \
           qdeclarativeservice.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += resource.qrc

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
