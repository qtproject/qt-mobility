HEADERS = bearercloud.h \
          cloud.h

SOURCES = main.cpp \
          bearercloud.cpp \
          cloud.cpp

RESOURCES = icons.qrc

TARGET = bearercloud

QT = core gui network svg

INCLUDEPATH += ../../src/bearer

include(../examples.pri)

qtAddLibrary(QtBearer)

CONFIG += console


symbian:TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData

macx: {
    contains(QT_CONFIG,qt_framework):LIBS += -framework QtBearer
    INCLUDEPATH += ../../
    contains(CONFIG, debug) {
    }
}
