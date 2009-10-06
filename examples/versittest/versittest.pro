
TEMPLATE = app
TARGET = VersitTest
QT += testlib xml
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += versittest.h

SOURCES += main.cpp versittest.cpp vcardcomparator.cpp testresultxmlparser.cpp

LIBS += -lQtContacts -lQtVersit

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfa329b3
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lws32 -lbafl
    TARGET.EPOCALLOWDLLDATA = 1
}
