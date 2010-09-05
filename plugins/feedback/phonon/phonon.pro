TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_phonon)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

QT += phonon
CONFIG += mobility
MOBILITY = feedback

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x200315FF
    TARGET = $${TARGET}$${QT_LIBINFIX}
    load(armcc_warnings)
    
    SymbianFeedbackPhononPlugin.sources = $${TARGET}.dll
    SymbianFeedbackPhononPlugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += SymbianFeedbackPhononPlugin
}
