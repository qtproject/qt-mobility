QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerplugins
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerplugins.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

