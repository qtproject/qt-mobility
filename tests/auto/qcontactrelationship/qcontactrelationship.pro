QT += testlib
TEMPLATE=app
TARGET=tst_qcontactrelationship
CONFIG+=testcase

include(../../../common.pri)

INCLUDEPATH += $$SOURCE_DIR/tests/auto

SOURCES  += tst_qcontactrelationship.cpp

qtAddLibrary(QtContacts)

# App local deployment
symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailactionfactory.dll
wince*:QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/$$mobilityDeployFilename(contacts_sendemailactionfactory).dll

QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
