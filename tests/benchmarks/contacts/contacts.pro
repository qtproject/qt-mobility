TEMPLATE = app
TARGET = tst_bm_contacts
CONFIG += testcase
QT += core

SOURCES += tst_bm_contacts.cpp

INCLUDEPATH += ../../../src/contacts\
               ../../../src/contacts/details \
               ../../../src/contacts/filters \
               ../../../src/contacts/requests

include(../../../common.pri)

CONFIG += mobility
MOBILITY = contacts

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}

