QT += testlib
TEMPLATE=app
TARGET=tst_qorganizeritemasync
CONFIG+=testcase

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/organizer \
               ../../../../src/organizer/details \
               ../../../../src/organizer/items \
               ../../../../src/organizer/requests \
               ../../../../src/organizer/filters

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = organizer

SOURCES  += tst_qorganizeritemasync.cpp
HEADERS += ../../qorganizermanagerdataholder.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
