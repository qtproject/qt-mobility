QT += testlib
TEMPLATE=app
TARGET=tst_qcontactactions
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactactions.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

