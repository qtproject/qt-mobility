TEMPLATE = app
TARGET = tst_maemo5om
QT += testlib
CONFIG += qtestlib

maemo5:
{
    include(../../../../../config.pri)
    INCLUDEPATH += .\inc
    #INCLUDEPATH += $$MAEMO5_PATHS
    #INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    #TODO: Remove these hacks!
    INCLUDEPATH += ../../../../../src/global
    INCLUDEPATH += ../../../../../src/organizer
    INCLUDEPATH += ../../../../../src/organizer/details
    INCLUDEPATH += ../../../../../src/organizer/engines
    INCLUDEPATH += ../../../../../src/organizer/filters
    INCLUDEPATH += ../../../../../src/organizer/items
    INCLUDEPATH += ../../../../../src/organizer/requests

    SOURCES += tst_maemo5om.cpp

    CONFIG += mobility
    MOBILITY = organizer

    #TARGET.CAPABILITY = ReadUserData \
    #                    WriteUserData

    # TODO: max heap size? is default enough for this test module?
    #TARGET.EPOCHEAPSIZE = 0x200000 0x4000000

    #:LIBS += -lqtorganizer
}
