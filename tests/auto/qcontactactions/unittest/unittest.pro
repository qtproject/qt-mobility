QT += testlib
TEMPLATE=app
TARGET=tst_qcontactactions
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

SOURCES  += tst_qcontactactions.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

QCONTACTACTIONS_PLUGINS_DEPLOY.sources = $$TEST_PLUGIN_DIR/*.dll
QCONTACTACTIONS_PLUGINS_DEPLOY.path = ./plugins

DEPLOYMENT += QCONTACTACTIONS_PLUGINS_DEPLOY
