TEMPLATE = lib
CONFIG += plugin
QT = core gui
TARGET = $$qtLibraryTarget(qtfeedback_simulator)
PLUGIN_TYPE=feedback

include(../../../common.pri)

HEADERS += \
    qfeedback.h \
    connection_p.h \
    qfeedbackdata_simulator_p.h
SOURCES += \
    qfeedback.cpp \
    connection.cpp \
    qfeedbackdata_simulator.cpp

INCLUDEPATH += $$SOURCE_DIR/src/feedback

INCLUDEPATH += $$SOURCE_DIR/src/mobilitysimulator
qtAddLibrary(QtMobilitySimulator)

CONFIG += mobility
MOBILITY = feedback
