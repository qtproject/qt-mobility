TEMPLATE = app
TARGET = tst_bm_contacts
CONFIG += testcase

SOURCES += tst_bm_contacts.cpp

QT += core \
      network

INCLUDEPATH += ../../../src/contacts\
                ../../../src/contacts/details \
                ../../../src/contacts/filters

include(../../../common.pri)

CONFIG += mobility
MOBILITY = contacts


EXTRAPLUGINS = \
    $$mobilityDeployFilename(qtcontacts_symbian)

symbian:QCONTACT_PLUGINS_DEPLOY.sources = $$join(EXTRAPLUGINS, ".dll ", " ", ".dll")
QCONTACT_PLUGINS_DEPLOY.path = ./plugins/contacts
DEPLOYMENT += QCONTACT_PLUGINS_DEPLOY


symbian {
    LIBS += -lcntmodel
    TARGET.CAPABILITY = All -TCB
}

maemo {
    QT += dbus xml gui
}
