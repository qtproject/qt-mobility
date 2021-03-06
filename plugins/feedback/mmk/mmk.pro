include(../../../features/utils.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$mobilityPluginTarget(qtfeedback_mmk)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

CONFIG += mobility
MOBILITY = feedback multimedia

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = $$mobilityUID(0x200315FF)
    load(armcc_warnings)

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}
