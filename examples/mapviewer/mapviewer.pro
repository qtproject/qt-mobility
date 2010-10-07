QT += xml network svg
TARGET = MapViewer
TEMPLATE = app 
INCLUDEPATH += ../../src/global \
               ../../src/bearer \
               ../../src/location \
               ../../src/location/maps
SOURCES += main.cpp \
           mapwidget.cpp \
           mainwindow.cpp
           
HEADERS += mapwidget.h \
           mainwindow.h

include(../examples.pri)

CONFIG += mobility
MOBILITY = location bearer
#contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7){
#    MOBILITY += bearer
#    INCLUDEPATH += ../../src/bearer
#}

symbian: { 
    TARGET.CAPABILITY = Location \
                        NetworkServices \
                        ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData
}
