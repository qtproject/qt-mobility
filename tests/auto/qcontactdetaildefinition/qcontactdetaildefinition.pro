QT += testlib
TEMPLATE=app
TARGET=tst_qcontactdetaildefinition
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactdetaildefinition.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

