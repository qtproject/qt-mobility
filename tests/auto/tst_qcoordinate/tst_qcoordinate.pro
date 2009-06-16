TEMPLATE = app
TARGET = tst_qcoordinate
CONFIG+=testcase
QT += testlib

include (../../../location.pri)

# Input 
SOURCES += tst_qcoordinate.cpp

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

