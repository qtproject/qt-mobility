#-------------------------------------------------
#
# Project created by QtCreator 2010-11-26T13:05:27
#
#-------------------------------------------------

QT       += core gui network

TARGET = mapbox
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mapbox.cpp \
    statswidget.cpp \
    boxgraphicsscene.cpp \
    performance.cpp

HEADERS  += mainwindow.h \
    mapbox.h \
    statswidget.h \
    boxgraphicsscene.h \
    performance.h

INCLUDEPATH += ../../src/global \
               ../../src/location \
               ../../src/location/maps

include("../../common.pri")

CONFIG += mobility
MOBILITY = location

win32 {
LIBS += -lpsapi
}
