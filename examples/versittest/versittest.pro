TEMPLATE = app
TARGET = 
QT += testlib \
    xml
CONFIG += qtestlib
include(../../common.pri)
DEPENDPATH += .
INCLUDEPATH += . \
    ../../versit

# Input
HEADERS += versittest.h \
    testconfiguration.h
SOURCES += main.cpp \
    versittest.cpp \
    vcardcomparator.cpp \
    testresultxmlparser.cpp \
    testconfiguration.cpp
qtAddLibrary(QtContacts)
qtAddLibrary(QtVersit)
symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
