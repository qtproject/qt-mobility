QT += testlib
TEMPLATE=app
TARGET=tst_qcontactasync
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactasync.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

# App local deployment
symbian:QCONTACTASYNC_PLUGINS_DEPLOY.sources = contacts_maliciousplugin.dll
wince:QCONTACTASYNC_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts_maliciousplugin.dll

QCONTACTASYNC_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTASYNC_PLUGINS_DEPLOY
