QT += testlib
TEMPLATE=app
TARGET=tst_qcontact
CONFIG+=testcase

include(../../../common.pri)

SOURCES  += tst_qcontact.cpp
LIBS += -lQtContacts

