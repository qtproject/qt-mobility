QT += core gui network
TARGET = mapbox
TEMPLATE = app
INCLUDEPATH += ../../../src/global \
               ../../../src/location \
               ../../../src/location/maps \
               ../../../src/location/maps/tiled

SOURCES +=  main.cpp \
            mainwindow.cpp \
            mapbox.cpp \
            statswidget.cpp \
            boxgraphicsscene.cpp \
            performance.cpp

HEADERS  +=     mainwindow.h \
                mapbox.h \
                statswidget.h \
                boxgraphicsscene.h \
                performance.h

include(../../../common.pri)

CONFIG += mobility
MOBILITY = location
equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7){
    MOBILITY += bearer
    INCLUDEPATH += ../../../src/bearer
} else {
    # use Bearer Management classes in QtNetwork module
}

win32 {
    LIBS += -lpsapi
}
