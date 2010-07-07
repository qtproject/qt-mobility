TARGET = tst_qgallerytrackeritemlist
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerytrackeritemlist.cpp

CONFIG += mobility
MOBILITY = gallery

QT += dbus

