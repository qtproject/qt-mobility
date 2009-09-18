TEMPLATE=app
TARGET=sfw-kinetic-example
INCLUDEPATH += ../../../serviceframework

include(../../../common.pri)

QT += gui declarative

# Input 
HEADERS += ../sfwexample.h
SOURCES += ../sfwexample.cpp \
           ../main.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
