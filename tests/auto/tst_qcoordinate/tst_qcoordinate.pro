TEMPLATE = app
TARGET = tst_qcoordinate

include (../../../location.pri)
include (../autotests.pri)

# Input 
SOURCES += tst_qcoordinate.cpp

symbian {
        TARGET.CAPABILITY = ALL -TCB
}
