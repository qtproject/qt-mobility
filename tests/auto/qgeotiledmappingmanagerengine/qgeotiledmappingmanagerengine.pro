TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeotiledmappingmanagerengine

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps \
                ../../../src/location/maps/tiled

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           ../pixelindex/pixelindexengine.h

SOURCES += tst_qgeotiledmappingmanagerengine.cpp \
           ../pixelindex/pixelindexengine.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
