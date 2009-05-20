TEMPLATE = app
TARGET = servicebrowser
INCLUDEPATH += ../../servicefw/serviceframework/servicehandler \
               ../../servicefw/serviceframework/serviceresolution \
               ../../servicefw/serviceframework

include(../../common.pri)

QT += gui

# Input 
HEADERS += servicebrowser.h
SOURCES += servicebrowser.cpp \
           main.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
