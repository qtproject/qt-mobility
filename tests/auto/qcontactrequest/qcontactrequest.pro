QT += testlib
TEMPLATE=app
TARGET=tst_qcontactrequest
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactrequest.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

