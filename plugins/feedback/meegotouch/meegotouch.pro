TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_meegotouch)
PLUGIN_TYPE = feedback
include(../../../common.pri)
HEADERS += qfeedback.h
SOURCES += qfeedback.cpp
INCLUDEPATH += $$SOURCE_DIR/src/feedback
LIBS += -lmeegotouchcore
CONFIG += mobility
MOBILITY = feedback
target.path = $$QT_MOBILITY_PREFIX/plugins/feedback
message($$TARGET)
INSTALLS += target
