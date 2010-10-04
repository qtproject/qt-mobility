QT       += network

TARGET = qsysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h
RESOURCES = examples.qrc

INCLUDEPATH += ../../src/systeminfo

include(../examples.pri)
CONFIG += mobility
MOBILITY = systeminfo

CONFIG += console

FORMS +=  dialog_small_landscape.ui

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment Location  ReadDeviceData
    TARGET.UID3 = 0x2002ac7e
}

