QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins_deprecated
CONFIG+=testcase

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts
SOURCES  += tst_qcontactmanagerplugins.cpp

TESTDLLS = \
    $$mobilityDeployFilename(contacts_testdummy_deprecated) \
    $$mobilityDeployFilename(contacts_testotherdummy_deprecated) \
    $$mobilityDeployFilename(contacts_testdummycopy_deprecated) \
    $$mobilityDeployFilename(contacts_testdummyinvalid_deprecated) \
    $$mobilityDeployFilename(contacts_testdummyempty_deprecated) \
    $$mobilityDeployFilename(contacts_testdummymemory_deprecated)

# App local deployment
symbian|wince* {
    symbian:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, ".dll ", " ", ".dll")
    wince*:QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, ".dll $$OUTPUT_DIR/build/tests/bin/plugins/contacts/", "$$OUTPUT_DIR/build/tests/bin/plugins/contacts/", ".dll")
    QCONTACTMANAGER_PLUGINS_DEPLOY.path = ./plugins/contacts
    DEPLOYMENT += QCONTACTMANAGER_PLUGINS_DEPLOY
}

# all else, we need to copy and install the DLLs to the right place (in a subdir for this test)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
