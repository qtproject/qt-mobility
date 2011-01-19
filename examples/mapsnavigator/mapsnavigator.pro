QT += network

TARGET = MapsNavigator
TEMPLATE = app

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

HEADERS += \
    mapswidget.h

SOURCES += \
    mapswidget.cpp \
    main.cpp
