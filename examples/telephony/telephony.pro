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

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment Location ReadDeviceData
    TARGET.UID3 = 0x2002BFD0
}
