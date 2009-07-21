QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanager
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactmanager.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

