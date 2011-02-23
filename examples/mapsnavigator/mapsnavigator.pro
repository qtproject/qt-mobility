QT += network

TARGET = MapsNavigator
TEMPLATE = app

INCLUDEPATH += ../../src/location \
               ../../src/location/maps

SOURCES += main.cpp\
           mainwindow.cpp \
           markerlist.cpp \
           searchwidget.cpp \
           mapswidget.cpp \
           directionswidget.cpp \

HEADERS  += mainwindow.h \
            markerlist.h \
            searchwidget.h \
            mapswidget.h \
            directionswidget.h \

RESOURCES += mapsnavigator.qrc

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
}
