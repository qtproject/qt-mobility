TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoinfosources_wince

include (../../../common.pri)

INCLUDEPATH += ../../../src/location
DEPENDPATH += ../../../buid/Debug/bin

# Input 
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h
SOURCES += ../qgeocoordinate/qlocationtestutils.cpp \
           tst_qgeoinfosources_wince.cpp

CONFIG += mobility
MOBILITY = location

