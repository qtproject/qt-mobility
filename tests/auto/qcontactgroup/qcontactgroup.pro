QT += testlib
TEMPLATE=app
TARGET=tst_qcontactgroup
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactgroup.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

