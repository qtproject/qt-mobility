TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_simulator)
PLUGIN_TYPE=organizer

CONFIG += mobility
MOBILITY = organizer

include(../../../common.pri)

INCLUDEPATH += ../../../src/mobilitysimulator
qtAddLibrary(QtMobilitySimulator)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

HEADERS += \
        qorganizersimulator_p.h \
        connection_p.h \
        qorganizerdata_simulator_p.h
SOURCES += \
        qorganizersimulator.cpp \
        connection.cpp \
        qorganizerdata_simulator.cpp
