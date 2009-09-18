#-------------------------------------------------
#
# Project created by QtCreator 2009-08-06T10:02:21
#
#-------------------------------------------------

QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

INCLUDEPATH += ../../systeminfo

include(../../common.pri)
qtAddLibrary(QtSystemInfo)

CONFIG += console
