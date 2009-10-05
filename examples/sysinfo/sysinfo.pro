QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

INCLUDEPATH += ../../systeminfo

include(../examples.pri)
qtAddLibrary(QtSystemInfo)

CONFIG += console
