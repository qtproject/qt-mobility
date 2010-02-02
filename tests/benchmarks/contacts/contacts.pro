TEMPLATE = app
TARGET = tst_contacts
CONFIG += testcase

SOURCES += tst_contacts.cpp

QT += core \
      network

LIBS += -L../../../lib -lQtDBus -lQtXml -lQtGui
INCLUDEPATH += ../../../src/contacts

include(../../../common.pri)

CONFIG += mobility

MOBILITY = contacts

symbian {
    TARGET.CAPABILITY = All -TCB
}
