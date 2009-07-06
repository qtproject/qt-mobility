QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetail
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetail.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

