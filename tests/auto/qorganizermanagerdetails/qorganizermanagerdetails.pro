QT += testlib
TEMPLATE=app
TARGET=tst_qorganizermanagerdetails
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
               ../../../src/organizer/items \
               ../../../src/organizer/details \
               ../../../src/organizer/requests \
               ../../../src/organizer/filters \
               ../../../src/organizer/engines
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = organizer
SOURCES  += tst_qorganizermanagerdetails.cpp
HEADERS += ../qorganizermanagerdataholder.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
