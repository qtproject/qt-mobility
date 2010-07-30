TEMPLATE = app
TARGET = tst_recurringitems
QT += testlib
CONFIG += qtestlib

symbian:
{
    INCLUDEPATH += ./inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_recurringItems.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
