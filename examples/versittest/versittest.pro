TEMPLATE = app
TARGET = versittest 
QT += xml testlib
CONFIG += qtestlib
DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/versit \
               ../../src/contacts \
               ../../src/contacts/requests \
               ../../src/contacts/details \
               ../../src/contacts/filters

qtAddLibrary(QtContacts)
qtAddLibrary(QtVersit)

# Input
HEADERS += versittest.h \
    testconfiguration.h \
    vcardcomparator.h \
    testresultxmlparser.h
SOURCES += main.cpp \
    versittest.cpp \
    vcardcomparator.cpp \
    testresultxmlparser.cpp \
    testconfiguration.cpp

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}

include(../examples.pri)
