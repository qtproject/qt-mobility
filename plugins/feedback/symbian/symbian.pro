TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_symbian)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback_symbian.h
SOURCES += qfeedback_symbian.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

CONFIG += mobility
MOBILITY = feedback


symbian {
    contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2) {
        # S60 3rd edition does not support theme effects, nor does it support
        # envelope modifiers to custom effects (attack and fade).
    } else {
        # S60 5th edition supports theme effects, but it does not support
        # envelope modifiers to custom effects (attack and fade).
        DEFINES += HAS_THEME_EFFECTS
        SOURCES += qfeedback_themed.cpp
        LIBS += -ltouchfeedback
    }

    contains(advancedtouchfeedback_enabled, yes) {
        # this uses the same lib and headers as touchfeedback
        # but on some platforms (S^3) includes more theme effects.
        DEFINES += HAS_ADVANCED_TACTILE_SUPPORT
    }

    contains(chwrmhaptics_enabled, yes) {
        DEFINES += HAS_FILE_EFFECTS
        DEFINES += HAS_CUSTOM_EFFECTS
        DEFINES += HAS_ENVELOPE_SUPPORT
        DEFINES += USE_CHWRMHAPTICS_PLZ
        SOURCES += qfeedback_chwrmhaptics.cpp
        LIBS += -lhwrmhapticsclient
    } else {
        DEFINES += HAS_CUSTOM_EFFECTS
        DEFINES += USE_CHWRMVIBRA_PLZ
        SOURCES += qfeedback_chwrmvibra.cpp
        LIBS += -lhwrmvibraclient
    }

    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3=0x200315FE
    TARGET = $${TARGET}$${QT_LIBINFIX}

    load(armcc_warnings)

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin

}
