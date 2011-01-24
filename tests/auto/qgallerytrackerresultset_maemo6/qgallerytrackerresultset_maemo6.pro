TARGET = tst_qgallerytrackerresultset
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerytrackerresultset.cpp

CONFIG += mobility
MOBILITY = gallery

QT += dbus
