QT += network

TARGET = mapsdemo
TEMPLATE = app

RESOURCES += mapsdemo.qrc

include(../examples.pri)

INCLUDEPATH += ../../src/location \
               ../../src/location/maps \
               ../../src/location/maps/tiled \
               ../../src/location/landmarks

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
    mapswidget.h \
    marker.h \
    mainwindow.h \
    searchdialog.h \
    markerdialog.h \
    navigatedialog.h \
    navigator.h

SOURCES += \
    mapswidget.cpp \
    main.cpp \
    marker.cpp \
    mainwindow.cpp \
    searchdialog.cpp \
    markerdialog.cpp \
    navigatedialog.cpp \
    navigator.cpp
