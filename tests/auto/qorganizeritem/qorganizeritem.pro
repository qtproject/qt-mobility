QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritem
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/details \
               ../../../src/organizer/requests \
               ../../../src/organizer/items \
               ../../../src/organizer/filters

INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = organizer

SOURCES  += tst_qorganizeritem.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
