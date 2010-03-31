TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL
symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002BFC0

    ### Sensors
    # Main library
    SENSORS_DEPLOYMENT.sources = QtSensors.dll
    SENSORS_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += SENSORS_DEPLOYMENT
}

CONFIG+=strict_flags

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS += \
           qsensorbackend.h\
           qsensormanager.h\
           qsensorplugin.h\

PRIVATE_HEADERS += \
           qsensorpluginloader_p.h\

SOURCES += qsensorbackend.cpp\
           qsensormanager.cpp\
           qsensorplugin.cpp\
           qsensorpluginloader.cpp\

# 3 files per sensor (including QSensor)
SENSORS=\
    qsensor\
    qaccelerometer\
    qambientlightsensor\
    qcompass\
    qmagnetometer\
    qorientationsensor\
    qproximitysensor\
    qrotationsensor\
    qtapsensor\

for(s,SENSORS) {
    # Client API
    PUBLIC_HEADERS += $${s}.h
    SOURCES        += $${s}.cpp
    # Private header
    PRIVATE_HEADERS += $${s}_p.h
}

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

CONFIG += middleware
include(../../features/deploy.pri)
