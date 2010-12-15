TEMPLATE = app
CONFIG += testcase
TARGET = tst_qgeotiledmapdata

include(../../../common.pri)

INCLUDEPATH += ../../../src/location \
	       ../../../src/location/maps \
               ../../../src/location/maps/tiled

HEADERS += ../qlocationtestutils_p.h \
           ../pixelindex/pixelindexengine.h

SOURCES += tst_qgeotiledmapdata.cpp \
           ../pixelindex/pixelindexengine.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
  INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
  TARGET.CAPABILITY = ALL -TCB
}
      
