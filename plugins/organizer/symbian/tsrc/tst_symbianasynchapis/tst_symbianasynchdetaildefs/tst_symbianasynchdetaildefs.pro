



TEMPLATE = app
TARGET = tst_symbianasynchdetaildefs
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianasynchdetaildefs.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
