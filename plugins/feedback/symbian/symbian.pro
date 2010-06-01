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
LIBS += -lQtFeedback -ltouchfeedback -lhwrmvibraclient
CONFIG += mobility
MOBILITY = feedback

