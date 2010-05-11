TEMPLATE = lib
TARGET = QtMobilitySimulator

!simulator: error(The QtMobilitySimulator library requires Simulator Qt to build.)

INCLUDEPATH += .
isEmpty(QT_NOKIA_SDK_PATH): QT_NOKIA_SDK_PATH = $$(QT_NOKIA_SDK_PATH)
isEmpty(QT_NOKIA_SDK_PATH): error(Please call qmake with QT_NOKIA_SDK_PATH=<path to nokia-sdk source directory>)

QT += network
include(../../common.pri)

# Input
HEADERS += mobilitysimulatorglobal.h \
    mobilityconnection_p.h
SOURCES += \
    mobilityconnection.cpp \
    $$QT_NOKIA_SDK_PATH/src/gui/kernel/qsimulatordata.cpp

DEFINES += QT_BUILD_MOBILITYSIMULATOR_LIB QT_MAKEDLL

include (../../features/deploy.pri)
