



TEMPLATE = app
TARGET = tst_symbianasynchremoveitems
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianasynchremoveitems.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
