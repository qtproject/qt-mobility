QT += testlib
TEMPLATE=app
TARGET=tst_qcontact
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontact.cpp

qtAddLibraryFixMe(QtContacts)

# App local deployment
symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailactionfactory.dll
wince*:QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/$$mobilityDeployFilename(contacts_sendemailactionfactory)

QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
