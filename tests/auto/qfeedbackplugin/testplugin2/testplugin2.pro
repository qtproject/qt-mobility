TEMPLATE = lib
CONFIG += plugin testplugin
QT = core
# named atestplugin to hopefully be loaded before the other ones
TARGET = $$qtLibraryTarget(qtfeedback_atestplugin)
PLUGIN_TYPE=feedback

include(../../../../common.pri)

HEADERS += qfeedbacktestplugin.h
SOURCES += qfeedbacktestplugin.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

CONFIG += mobility
MOBILITY = feedback

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET = $${TARGET}$${QT_LIBINFIX}
    load(armcc_warnings)

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}
