TARGET = tst_qgallerycountrequest
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerycountrequest.cpp

CONFIG += mobility
MOBILITY = gallery

