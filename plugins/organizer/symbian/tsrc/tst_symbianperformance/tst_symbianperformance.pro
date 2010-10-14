TEMPLATE = app
TARGET = tst_symbianperformance
QT += testlib
CONFIG += qtestlib

symbian:
{
    INCLUDEPATH += ./inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianperformance.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    # Performance testing potentially takes a lot of memory
    TARGET.EPOCHEAPSIZE = 0x200000 0x4000000

    LIBS += -lqtorganizer
}
