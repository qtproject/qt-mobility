QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetails
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetails.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

