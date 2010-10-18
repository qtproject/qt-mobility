QT += testlib
TEMPLATE=app
TARGET=tst_qorganizercollection
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

SOURCES  += tst_qorganizercollection.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
