TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_maemo5)
PLUGIN_TYPE=organizer

include(../../../common.pri)

HEADERS += \
        qorganizermaemo5_p.h
SOURCES += \
        qorganizermaemo5.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target
CONFIG += mobility link_pkgconfig
MOBILITY = organizer
PKGCONFIG += calendar-backend-dev
