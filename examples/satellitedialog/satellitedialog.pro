TEMPLATE = app

SOURCES += qgeosatellitedialog.cpp main.cpp
HEADERS += qgeosatellitedialog.h 

INCLUDEPATH += ../../src/location \
                ../../src/global

include(../examples.pri)

CONFIG += mobility
MOBILITY += location

symbian: {
    TARGET.CAPABILITY = Location
}

