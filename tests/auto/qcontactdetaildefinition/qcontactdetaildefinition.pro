QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetaildefinition
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = contacts
SOURCES  += tst_qcontactdetaildefinition.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
