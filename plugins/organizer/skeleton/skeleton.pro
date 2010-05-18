TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_skeleton)
PLUGIN_TYPE=organizer

include(../../../common.pri)

HEADERS += \
        qorganizerskeleton_p.h
SOURCES += \
        qorganizerskeleton.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target
CONFIG += mobility
MOBILITY = organizer

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details
