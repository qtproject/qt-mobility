



TEMPLATE = app
TARGET = tst_asynchapis
QT += testlib
CONFIG += qtestlib

symbian:
{
	INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_asynchapis.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    LIBS += -lqtorganizer
}
