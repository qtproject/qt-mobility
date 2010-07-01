TEMPLATE = app
TARGET = tst_symbianom
QT += testlib
CONFIG += qtestlib

symbian:
{
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_symbianom.cpp

    CONFIG += mobility
    MOBILITY = organizer

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    # TODO: max heap size? is default enough for this test module?
    #TARGET.EPOCHEAPSIZE = 0x200000 0x4000000

    LIBS += -lqtorganizer
}
