TEMPLATE=app
TARGET=sfw-kinetic-example
INCLUDEPATH += ../../../serviceframework

include(../../examples.pri)

QT += gui declarative

# Input 
HEADERS += ../sfwexample.h
SOURCES += ../sfwexample.cpp \
           ../main.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
