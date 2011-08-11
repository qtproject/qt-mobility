QT += testlib
TEMPLATE=app
TARGET=tst_qcontactrelationship
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

INCLUDEPATH += ../../../src/contacts \
               ../../../src/contacts/details \
               ../../../src/contacts/requests \
               ../../../src/contacts/filters

INCLUDEPATH += ../

SOURCES  += tst_qcontactrelationship.cpp

CONFIG += mobility
MOBILITY = contacts

# App local deployment
symbian|wince* {
    symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailaction.dll
    wince*:QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/bin/plugins/contacts/$$mobilityDeployFilename(contacts_sendemailaction).dll

    QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

    DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY
}

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
