QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanager
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontactmanager.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

