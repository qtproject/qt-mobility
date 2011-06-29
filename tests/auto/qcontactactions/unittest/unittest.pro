QT += testlib
TEMPLATE=app
TARGET=tst_qcontactactions
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters \
               ../../../../src/serviceframework

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts serviceframework
SOURCES  += tst_qcontactactions.cpp

# App local deployment
symbian|wince* {
    symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailaction.dll
    wince*:QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/bin/plugins/contacts/$$mobilityDeployFilename(contacts_sendemailaction).dll

    QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

    DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY
}

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}

maemo*:CONFIG += insignificant_test
