
TEMPLATE = app
TARGET =
QT += testlib xml
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += versittest.h

SOURCES += main.cpp versittest.cpp vcardcomparator.cpp testresultxmlparser.cpp

LIBS += -lQtContacts -lQtVersit

symbian: {
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lws32 -lbafl
}
