QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanager
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = contacts
contains(mobility_modules,versit) {
    MOBILITY += versit
    DEFINES += USE_VERSIT_PLZ
    INCLUDEPATH += ../../../src/versit
}
SOURCES  += tst_qcontactmanager.cpp
HEADERS += ../qcontactmanagerdataholder.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
