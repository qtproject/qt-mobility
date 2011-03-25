QT       += core gui

TARGET = qsystemalignedtimer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include(../mobility_examples.pri)
CONFIG += mobility console
MOBILITY = systeminfo

