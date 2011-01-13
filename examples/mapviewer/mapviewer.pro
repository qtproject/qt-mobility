QT += xml network svg
TARGET = MapViewer
TEMPLATE = app
INCLUDEPATH += ../../src/global \
               ../../src/location \
               ../../src/location/maps
SOURCES += main.cpp \
           mainwindow.cpp \
           mapbox.cpp \
           geomap.cpp \
           mapbox_menuhandlers.cpp \
           program_options.cpp

HEADERS += mainwindow.h \
           mapbox.h \
    geomap.h \
    program_options.h

include(../examples.pri)

CONFIG += mobility
MOBILITY = location
equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7){
    MOBILITY += bearer
    INCLUDEPATH += ../../src/bearer
} else {
    # use Bearer Management classes in QtNetwork module
}

symbian: {
    TARGET.CAPABILITY = Location \
                        NetworkServices \
                        ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData

    TARGET.EPOCHEAPSIZE = 1048576 67108864
    TARGET.EPOCSTACKSIZE = 0xA000
}

unix:!symbian {
    QMAKE_CXXFLAGS += -DGIT_VERSION=\\\"$(shell git describe)\\\"
}
