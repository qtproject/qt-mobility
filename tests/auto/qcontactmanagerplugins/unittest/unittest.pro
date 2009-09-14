QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerplugins.cpp
LIBS += -lQtContacts


TESTDLLS = \
    contacts_testdummy \
    contacts_testotherdummy \
    contacts_testdummycopy \
    contacts_testdummyinvalid \
    contacts_testdummyempty \
    contacts_testdummymemory

# App local deployment
symbian|wince* {
    symbian:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, ".dll ", " ", ".dll")
    wince* {
        DLL_SUFFIX = ".dll"
        CONFIG(debug, debug|release): DLL_SUFFIX="d.dll"
        QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, "$$DLL_SUFFIX $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/", "$$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/", "$$DLL_SUFFIX")
    }
    QCONTACTMANAGER_PLUGINS_DEPLOY.path = ./plugins/contacts
    DEPLOYMENT += QCONTACTMANAGER_PLUGINS_DEPLOY
}

# all else, we need to copy and install the DLLs to the right place (in a subdir for this test)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
