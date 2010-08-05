



TEMPLATE = app
TARGET = tst_fetchitems
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_fetchitems.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
