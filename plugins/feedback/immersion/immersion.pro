TEMPLATE = lib
CONFIG += plugin
QT = core
TARGET = $$qtLibraryTarget(qtfeedback_immersion)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

LIBS += -limmvibe
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
    symbianplugin.path = /resource/qt/plugins/feedback
    DEPLOYMENT += symbianplugin
}

target.path=$$QT_MOBILITY_PREFIX/plugins/feedback
INSTALLS+=target
