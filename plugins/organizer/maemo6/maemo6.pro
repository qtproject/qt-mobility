TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_maemo6)
PLUGIN_TYPE=organizer

include(../../../common.pri)

HEADERS += \
        qorganizermaemo6_p.h
SOURCES += \
        qorganizermaemo6.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target
CONFIG += mobility link_pkgconfig
MOBILITY = organizer
PKGCONFIG += 
