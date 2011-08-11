TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeomapobjectinfo

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps \
                ../qgeomapobjectplugin
DEPENDPATH += ../../../buid/Debug/bin

# Input
SOURCES += tst_qgeomapobjectinfo.cpp \
    tst_qgeomapdata.cpp \
    tst_qgeomappingmanagerengineinherit.cpp \
    tst_qgeomapobjectinfoinherit.cpp
            

HEADERS += \
    tst_qgeomapdata.h \
    qgeomappingmanagerengineinherit.h \
    tst_qgeomapobjectinfoinherit.h

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}

CONFIG += middleware
include(../../../features/deploy.pri)
