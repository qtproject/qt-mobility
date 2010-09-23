QT += xml network
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
contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7){
    MOBILITY += bearer
    INCLUDEPATH += ../../src/bearer
}

symbian: { 
    TARGET.CAPABILITY = Location \
                        NetworkServices \
                        ReadUserData \
                        WriteUserData
}
