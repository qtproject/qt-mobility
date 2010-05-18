TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_skeleton)
PLUGIN_TYPE=organizer

CONFIG += mobility
MOBILITY = organizer

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

HEADERS += \
        qorganizerskeleton_p.h
SOURCES += \
        qorganizerskeleton.cpp

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

symbian {
    load(data_caging_paths)

    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
#    TARGET.UID3 = 0x2002AC7B

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    target.path = /sys/bin

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}

