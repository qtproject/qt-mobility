QT += xml \
    network
TARGET = MapViewer
TEMPLATE = app \
    thread
INCLUDEPATH += ../../src/global \
    ../../src/location \
    ../../include/QtmLocation
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui
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
RESOURCES += images.qrc
