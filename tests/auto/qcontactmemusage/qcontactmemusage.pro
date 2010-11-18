QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmemusage
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters
INCLUDEPATH += ../

CONFIG += mobility
MOBILITY = contacts
SOURCES  += tst_qcontactmemusage.cpp

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
