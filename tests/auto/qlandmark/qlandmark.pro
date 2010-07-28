TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmark

include (../../../common.pri)

INCLUDEPATH += ../../../src/landmarks \
                 ../../../src/location

# Input 
SOURCES += tst_qlandmark.cpp

CONFIG += mobility
MOBILITY = landmarks location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
