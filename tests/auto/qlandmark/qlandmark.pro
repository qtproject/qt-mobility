TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmark

include (../../../common.pri)

INCLUDEPATH +=  ../../../src/location \
                ../../../src/location/landmarks

# Input 
SOURCES += tst_qlandmark.cpp

CONFIG += mobility
MOBILITY = landmarks location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
