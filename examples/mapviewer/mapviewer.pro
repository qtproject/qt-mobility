QT += xml \
    network
TARGET = MapViewer
TEMPLATE = app 
INCLUDEPATH += ../../src/global \
    ../../src/location \
    ../../src/location/maps
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
include(../examples.pri)
CONFIG += mobility
MOBILITY = location
symbian: { 
    MOBILITY += bearer
    TARGET.CAPABILITY = Location \
        NetworkServices \
        ReadUserData \
        WriteUserData
}
