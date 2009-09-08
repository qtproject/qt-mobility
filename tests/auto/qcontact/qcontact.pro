QT += testlib
TEMPLATE=app
TARGET=tst_qcontact
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontact.cpp
LIBS += -lQtContacts

# App local deployment
symbian:QCONTACTACTION_PLUGINS_DEPLOY.sources = contacts_sendemailactionfactory.dll
wince* {
    DLL_SUFFIX = ".dll"
    CONFIG(debug, debug|release): DLL_SUFFIX="d.dll"
    QCONTACTACTION_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/contacts_sendemailactionfactory$$DLL_SUFFIX
}

QCONTACTACTION_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTACTION_PLUGINS_DEPLOY
