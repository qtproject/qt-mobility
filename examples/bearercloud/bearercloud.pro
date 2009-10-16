HEADERS = bearercloud.h \
          cloud.h

SOURCES = main.cpp \
          bearercloud.cpp \
          cloud.cpp

RESOURCES = icons.qrc

TARGET = bearercloud

QT = core gui network svg

INCLUDEPATH += ../../bearer

include(../examples.pri)

qtAddLibrary(QtBearer)

CONFIG += console

symbian {
    BEARERLIB.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/QtBearer.dll
    BEARERLIB.path = .
    DEPLOYMENT += BEARERLIB
}
