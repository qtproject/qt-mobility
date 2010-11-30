TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_mkcal)
PLUGIN_TYPE=organizer

CONFIG += mobility
MOBILITY = organizer

CONFIG += link_pkgconfig
PKGCONFIG += libmkcal

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

HEADERS += \
        mkcalengine.h \
        qorganizerasynchmanager.h \
        mkcalid.h
SOURCES += \
        mkcalengine.cpp \
        qorganizerasynchmanager.cpp

