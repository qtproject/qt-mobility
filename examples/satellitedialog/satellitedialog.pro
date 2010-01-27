TEMPLATE = app

SOURCES += satellitedialog.cpp main.cpp
HEADERS += satellitedialog.h

INCLUDEPATH += ../../src/location \
                ../../src/global

include(../examples.pri)

CONFIG += mobility
MOBILITY += location

symbian: {
    TARGET.CAPABILITY = Location
}

