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

win32 {
    LIBS += -lpsapi
}
