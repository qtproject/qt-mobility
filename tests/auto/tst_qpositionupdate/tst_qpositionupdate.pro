TEMPLATE = app
TARGET = tst_qpositionupdate
CONFIG+=testcase
QT += testlib

include (../../../location.pri)

# Input 
SOURCES += tst_qpositionupdate.cpp

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

