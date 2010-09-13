TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_maemo5)
PLUGIN_TYPE = organizer

CONFIG += mobility
MOBILITY = organizer

CONFIG += link_pkgconfig
PKGCONFIG += calendar-backend

QT += sql

include(../../../common.pri)
INCLUDEPATH += ../../../src/organizer \
    ../../../src/organizer/items \
    ../../../src/organizer/requests \
    ../../../src/organizer/filters \
    ../../../src/organizer/details
HEADERS += qorganizermaemo5_p.h \
    qorganizerrecurrencetransform.h \
    qorganizeritemtransform.h \
    qorganizerasynchprocess.h \
    qorganizercaldbaccess.h
SOURCES += qorganizermaemo5.cpp \
    qorganizerrecurrencetransform.cpp \
    qorganizeritemtransform.cpp \
    qorganizerasynchprocess.cpp \
    qorganizercaldbaccess.cpp
