TEMPLATE = app
TARGET = qtlandmarksdocsample

include(../../../../common.pri)
INCLUDEPATH += ../../../../src/location

DESTDIR = $$QT_MOBILITY_BUILD_TREE/bin/examples
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility
MOBILITY += location

SOURCES += qtlandmarksdocsample.cpp
HEADERS += requestexample.h
