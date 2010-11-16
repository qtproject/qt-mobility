TEMPLATE = app
TARGET = tst_bm_organizer
CONFIG += testcase
QT += core

SOURCES += tst_bm_organizer.cpp

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/items \
               ../../../src/organizer/details \
               ../../../src/organizer/filters \
               ../../../src/organizer/requests

include(../../../common.pri)

CONFIG += mobility
MOBILITY = organizer

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}

