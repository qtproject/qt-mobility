QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerplugins.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

# App local deployment
symbian:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = \
    contacts_testdummy.dll \
    contacts_testotherdummy.dll \
    contacts_testdummycopy.dll \
    contacts_testdummyinvalid.dll \
    contacts_testdummyempty.dll \
    contacts_testdummymemory.dll

wince:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contact_test*.dll

QCONTACTMANAGER_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTMANAGER_PLUGINS_DEPLOY
