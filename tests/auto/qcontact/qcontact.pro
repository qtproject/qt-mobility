QT += testlib
TEMPLATE=app
TARGET=tst_qcontact
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontact.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

