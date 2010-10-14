



TEMPLATE = app
TARGET = tst_symbianasynchfetchitems
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += ./inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianasynchfetchitems.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
