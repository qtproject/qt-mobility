QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerdetails
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters \
               ../../../src/global
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = contacts

HEADERS += ../qcontactmanagerdataholder.h

SOURCES  += tst_qcontactmanagerdetails.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    contains(S60_VERSION,3.1) {
        DEFINES += SYMBIAN_BACKEND_S60_VERSION_31
        LIBS += -lcntmodel
    }
    contains(S60_VERSION,3.2) {
        DEFINES += SYMBIAN_BACKEND_S60_VERSION_32
        LIBS += -lcntmodel
    }
    contains(S60_VERSION,5.0) {
        DEFINES += SYMBIAN_BACKEND_S60_VERSION_50
        LIBS += -lcntmodel
    }
}
