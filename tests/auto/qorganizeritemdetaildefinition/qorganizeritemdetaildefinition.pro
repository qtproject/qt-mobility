QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritemdetaildefinition
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
SOURCES  += tst_qorganizeritemdetaildefinition.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
