TEMPLATE = app
TARGET = servicesdemo
INCLUDEPATH += ../../servicefw/serviceframework/servicehandler \
               ../../servicefw/serviceframework/serviceresolution \
               ../../servicefw/serviceframework

include(../../common.pri)

QT += gui

# Input 
HEADERS += mainwindow.h
SOURCES += mainwindow.cpp \
           main.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
