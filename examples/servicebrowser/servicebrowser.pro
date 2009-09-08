TEMPLATE = app
TARGET = servicebrowser
INCLUDEPATH += ../../serviceframework

include(../examples.pri)

QT += gui

# Input 
HEADERS += servicebrowser.h
SOURCES += servicebrowser.cpp \
           main.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
