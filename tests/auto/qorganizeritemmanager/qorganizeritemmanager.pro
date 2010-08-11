QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritemmanager
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/items \
               ../../../src/organizer/details \
               ../../../src/organizer/requests \
               ../../../src/organizer/filters
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = organizer
SOURCES  += tst_qorganizeritemmanager.cpp
HEADERS += ../qorganizeritemmanagerdataholder.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
