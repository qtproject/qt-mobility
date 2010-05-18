TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_maemo5)
PLUGIN_TYPE=organizer

CONFIG += mobility
MOBILITY = organizer

CONFIG += link_pkgconfig
PKGCONFIG += calendar-backend

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

HEADERS += \
        qorganizermaemo5_p.h
SOURCES += \
        qorganizermaemo5.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

