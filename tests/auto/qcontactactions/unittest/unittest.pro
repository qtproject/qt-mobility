QT += testlib
TEMPLATE=app
TARGET=tst_qcontactactions
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactactions.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests

LIBS += -lQtContacts

