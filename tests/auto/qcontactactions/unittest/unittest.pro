QT += testlib
TEMPLATE=app
TARGET=tst_qcontactactions
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

SOURCES  += tst_qcontactactions.cpp

LIBS += -lQtContacts

# App local deployment
symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailactionfactory.dll
wince:QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts_sendemailactionfactory.dll

QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY
