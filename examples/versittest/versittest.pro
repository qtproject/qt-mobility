TEMPLATE = app
TARGET = 
QT += testlib \
    xml
CONFIG += qtestlib
include(../../common.pri)
DEPENDPATH += .
INCLUDEPATH += . \
               ../../versit \
               ../../contacts \
               ../../contacts/requests \
               ../../contacts/details \
               ../../contacts/filters

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
