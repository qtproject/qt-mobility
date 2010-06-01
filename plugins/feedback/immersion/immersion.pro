TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_immersion)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

target.path=$$QT_MOBILITY_PREFIX/plugins/feedback
INSTALLS+=target
LIBS += -lImmVibe -lQtFeedback
CONFIG += mobility
MOBILITY = feedback

