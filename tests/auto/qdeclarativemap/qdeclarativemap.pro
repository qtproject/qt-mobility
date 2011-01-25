TEMPLATE=app
CONFIG+=testcase
include (../../../common.pri)
# prefix test binary with "tst_"
!contains(TARGET, ^tst_.*):TARGET = $$join(TARGET,,"tst_")
QT += testlib declarative
CONFIG += mobility

INCLUDEPATH += ../../../src/global
SOURCES += tst_qdeclarativemap.cpp

contains(mobility_modules,location) {
    MOBILITY += location
    QT *= network
    DEFINES += API_TEST_DECLARATIVE_LOCATION=1
    INCLUDEPATH += ../../../src/location
    INCLUDEPATH += ../../../plugins/declarative/location
    INCLUDEPATH += ../../../src/location/landmarks
    INCLUDEPATH += ../../../src/location/maps
    HEADERS += ../../../plugins/declarative/location/qdeclarativecoordinate_p.h
    HEADERS += ../../../plugins/declarative/location/qdeclarativegeoserviceprovider_p.h
    HEADERS += ../../../plugins/declarative/location/qdeclarativegeoaddress_p.h
    SOURCES += ../../../plugins/declarative/location/qdeclarativecoordinate.cpp
    SOURCES += ../../../plugins/declarative/location/qdeclarativegeoserviceprovider.cpp
    SOURCES += ../../../plugins/declarative/location/qdeclarativegeoaddress.cpp
}


symbian|wince* {
    symbian {
        TARGET.CAPABILITY = All -TCB
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    }
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
