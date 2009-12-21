TEMPLATE=app
TARGET=sfw-kinetic-example
INCLUDEPATH += ../../../src/serviceframework

include(../../examples.pri)

QT += gui declarative

# Input 
HEADERS += ../sfwexample.h
SOURCES += ../sfwexample.cpp \
           ../main.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
