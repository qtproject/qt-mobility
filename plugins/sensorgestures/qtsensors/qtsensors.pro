PLUGIN_TYPE = sensorgestures

CONFIG += plugin
TEMPLATE = lib

TARGET = $$qtLibraryTarget(qtsensorgestures_qtsensors)

include(../../../common.pri)
QT = core
CONFIG += mobility
MOBILITY += sensors

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/sensors/gestures
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/sensors
# Input

HEADERS +=\
    qcoversensorgesturerecognizer.h \
#    qdoubletapsensorgesturerecognizer.h \
    qhoversensorgesturerecognizer.h \
#    qfreefallsensorgesturerecognizer.h \
    qpickupsensorgesturerecognizer.h \
    qshake2recognizer.h \
    qslamgesturerecognizer.h \
    qturnoversensorgesturerecognizer.h \
    qtwistsensorgesturerecognizer.h \
    qwhipsensorgesturerecognizer.h \
    qtsensorgesturesensorhandler.h \
    qtsensorgestureplugin.h

SOURCES += \
    qcoversensorgesturerecognizer.cpp \
 #   qdoubletapsensorgesturerecognizer.cpp \
    qhoversensorgesturerecognizer.cpp \
 #   qfreefallsensorgesturerecognizer.cpp \
    qpickupsensorgesturerecognizer.cpp \
    qshake2recognizer.cpp \
    qslamgesturerecognizer.cpp \
    qturnoversensorgesturerecognizer.cpp \
    qtwistsensorgesturerecognizer.cpp \
    qwhipsensorgesturerecognizer.cpp \
    qtsensorgesturesensorhandler.cpp \
    qtsensorgestureplugin.cpp


CONFIG+=strict_flags
