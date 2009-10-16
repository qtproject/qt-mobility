TARGET = tst_qcrmlparser
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
CONFIG += testcase
QT = core xml
include(../../../common.pri)

# Input
SOURCES += ../../../context/qcrmlparser.cpp \ 
        tst_qcrmlparser.cpp
DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"

symbian:TARGET.CAPABILITY = ALL \
    -TCB
