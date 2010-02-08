TEMPLATE = app
TARGET = tst_contacts
CONFIG += testcase

SOURCES += tst_contacts.cpp

QT += core \
      network

LIBS += -L../../../lib

INCLUDEPATH += ../../../src/contacts

include(../../../common.pri)

CONFIG += mobility

MOBILITY = contacts

symbian {
	LIBS += -lcntmodel
    TARGET.CAPABILITY = All -TCB
}

maemo {
    LIBS += -lQtDBus -lQtXml -lQtGui
}
