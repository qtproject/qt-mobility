QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritemfilter
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/details \
               ../../../src/organizer/requests \
               ../../../src/organizer/items \
               ../../../src/organizer/filters
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = organizer

SOURCES  += tst_qorganizeritemfilter.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
