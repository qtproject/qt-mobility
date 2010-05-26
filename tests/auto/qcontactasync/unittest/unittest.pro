QT += testlib
TEMPLATE=app
TARGET=tst_qcontactasync
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/contacts \
               ../../../../src/contacts/details \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters

INCLUDEPATH += ../../

CONFIG += mobility
MOBILITY = contacts

SOURCES  += tst_qcontactasync.cpp
HEADERS += ../../qcontactmanagerdataholder.h

# App local deployment
symbian:QCONTACTASYNC_PLUGINS_DEPLOY.sources = contacts_maliciousplugin.dll
wince*: QCONTACTASYNC_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/bin/plugins/contacts/$$mobilityDeployFilename(contacts_maliciousplugin).dll

QCONTACTASYNC_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTASYNC_PLUGINS_DEPLOY

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData
}
