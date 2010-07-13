TEMPLATE=app
CONFIG+=testcase
include (../../../common.pri)
# prefix test binary with "tst_"
!contains(TARGET, ^tst_.*):TARGET = $$join(TARGET,,"tst_")
QT += testlib declarative

SOURCES += tst_qdeclarativeposition.cpp

INCLUDEPATH += ../../../src/location
INCLUDEPATH += ../../../src/global

CONFIG += mobility
MOBILITY += location

symbian|wince* {
    symbian {
        TARGET.CAPABILITY = All -TCB
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    }
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

RESOURCES += \
    locationdata.qrc
