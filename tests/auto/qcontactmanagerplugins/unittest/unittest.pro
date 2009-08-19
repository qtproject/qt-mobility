QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerplugins.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts


TESTDLLS = \
    contacts_testdummy.dll \
    contacts_testotherdummy.dll \
    contacts_testdummycopy.dll \
    contacts_testdummyinvalid.dll \
    contacts_testdummyempty.dll \
    contacts_testdummymemory.dll

# App local deployment
symbian|wince {
    symbian:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$TESTDLLS
    wince:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, " ", "$$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/")
    QCONTACTMANAGER_PLUGINS_DEPLOY.path = ./plugins/contacts
    DEPLOYMENT += QCONTACTMANAGER_PLUGINS_DEPLOY
}

# all else, we need to copy and install the DLLs to the right place (in a subdir for this test)


