TARGET = tst_qgalleryquerymodel
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgalleryquerymodel.cpp

CONFIG += mobility
MOBILITY = gallery
