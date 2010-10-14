QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritemdetails
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/details \
               ../../../src/organizer/requests \
               ../../../src/organizer/filters \
               ../../../src/organizer/items
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = organizer
SOURCES  += tst_qorganizeritemdetails.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
