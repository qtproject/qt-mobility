QT += testlib
TEMPLATE=app
TARGET=tst_qcontactasync
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

SOURCES  += tst_qcontactasync.cpp

LIBS += -lQtContacts

# App local deployment
symbian:QCONTACTASYNC_PLUGINS_DEPLOY.sources = contacts_maliciousplugin.dll
wince* {
    DLL_SUFFIX = ".dll"
    CONFIG(debug, debug|release): DLL_SUFFIX="d.dll"
    QCONTACTASYNC_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/contacts_maliciousplugin$$DLL_SUFFIX
}
QCONTACTASYNC_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTASYNC_PLUGINS_DEPLOY

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
