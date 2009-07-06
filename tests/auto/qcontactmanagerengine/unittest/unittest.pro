QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagerengine
CONFIG+=testcase

include(../../../../common.pri)

SOURCES  += tst_qcontactmanagerengine.cpp

INCLUDEPATH += $$SOURCE_DIR/contacts

LIBS += -lQtContacts

