TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_symbian)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

target.path=$$QT_MOBILITY_PREFIX/plugins/feedback
INSTALLS+=target
LIBS += -lQtFeedback -lhwrmvibraclient
CONFIG += mobility
MOBILITY = feedback


symbian {
    contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2) {
        DEFINES += NO_TACTILE_SUPPORT
    } else {
        LIBS += -ltouchfeedback
        contains(S60_VERSION, 5.2): DEFINES += ADVANCED_TACTILE_SUPPORT
    }
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET = $${TARGET}$${QT_LIBINFIX}
    load(armcc_warnings)
}
