



TEMPLATE = app
TARGET = tst_symbianasynchfetchiteminstance
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += ./inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianasynchfetchiteminstance.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
