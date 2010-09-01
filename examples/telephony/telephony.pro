# #####################################################################
# Telephony Mobility API
# #####################################################################

QT       += core gui
TEMPLATE = app
TARGET = callmonitor

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../src/telephony

include(../examples.pri)
CONFIG += mobility
MOBILITY = telephony
