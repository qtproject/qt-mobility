TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeotiledmapdata

include(../../../common.pri)

INCLUDEPATH += ../../../src/location \
	       ../../../src/location/maps \
               ../../../src/location/maps/tiled

HEADERS += ../qlocationtestutils_p.h \
           ../qgeotiledmappingmanagerengine/pixelindexengine.h

SOURCES += tst_qgeotiledmapdata.cpp \
           ../qgeotiledmappingmanagerengine/pixelindexengine.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
  INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
  TARGET.CAPABILITY = ALL -TCB
}
      
