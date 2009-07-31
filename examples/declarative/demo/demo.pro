TEMPLATE=app
TARGET=sfw-kinetic-example
INCLUDEPATH += ../../../serviceframework

include(../../../common.pri)

QT += gui declarative

# Input 
HEADERS += ../sfwexample.h
SOURCES += ../sfwexample.cpp \
           ../main.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
