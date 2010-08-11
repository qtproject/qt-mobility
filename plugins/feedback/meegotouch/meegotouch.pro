TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_meegotouch)
PLUGIN_TYPE = feedback

include(../../../common.pri)

CONFIG += mobility
MOBILITY = feedback

HEADERS += qfeedback.h
SOURCES += qfeedback.cpp
INCLUDEPATH += $$SOURCE_DIR/src/feedback

LIBS += -lmeegotouchcore
