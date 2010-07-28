TEMPLATE = lib
TARGET = QtMobilitySimulator

!simulator: error(The QtMobilitySimulator library requires Simulator Qt to build.)

INCLUDEPATH += .

QT += gui network
include(../../common.pri)

# Input
HEADERS += mobilitysimulatorglobal.h \
    mobilityconnection_p.h
SOURCES += \
    mobilityconnection.cpp

DEFINES += QT_BUILD_MOBILITYSIMULATOR_LIB QT_MAKEDLL

include (../../features/deploy.pri)
