QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins
include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerplugins.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

QCONTACTMANAGER_PLUGINS_DEPLOY.sources = $$TEST_PLUGIN_DIR/*.dll
QCONTACTMANAGER_PLUGINS_DEPLOY.path = ./plugins

DEPLOYMENT += QCONTACTMANAGER_PLUGINS_DEPLOY
