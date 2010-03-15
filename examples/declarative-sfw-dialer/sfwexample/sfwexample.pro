TEMPLATE=app
TARGET=declarative-sfw-dialer
INCLUDEPATH += ../../../src/serviceframework

include(../../examples.pri)

QT += gui declarative

# Input 
HEADERS += sfwexample.h
SOURCES += sfwexample.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += resource.qrc

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
