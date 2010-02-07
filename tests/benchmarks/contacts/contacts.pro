TEMPLATE = app
TARGET = tst_contacts
CONFIG += testcase

SOURCES += tst_contacts.cpp

QT += core \
      network

#LIBS += -L../../../lib

INCLUDEPATH += ../../../src/contacts\
                ../../../src/contacts/details \
                ../../../src/contacts/filters

include(../../../common.pri)

CONFIG += mobility

MOBILITY = contacts

symbian {
    TARGET.CAPABILITY = All -TCB
}

maemo {
    QT += dbus xml gui
    #LIBS += -lQtDBus -lQtXml -lQtGui
}
