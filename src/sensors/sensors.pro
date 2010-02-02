TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL
symbian:TARGET.EPOCALLOWDLLDATA = 1

STRICT=$$(STRICT)
equals(STRICT,1) {
    QMAKE_CXXFLAGS+=-Werror
    QMAKE_LFLAGS+=-Wl,-no-undefined
}

PUBLIC_HEADERS += \
           qsensorbackend.h\
           qsensorfactory.h\
           qsensormanager.h\
           qsensorplugin.h\

PRIVATE_HEADERS += \
           qsensorpluginloader_p.h\

SOURCES += qsensorbackend.cpp\
           qsensorfactory.cpp\
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

include(../../features/deploy.pri)
