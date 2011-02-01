#-------------------------------------------------
#
# Project created by QtCreator 2010-10-15T11:08:04
#
#-------------------------------------------------

QT       += core gui

TARGET = qsystemalignedtimer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include(../examples.pri)
CONFIG += mobility console
MOBILITY = systeminfo

