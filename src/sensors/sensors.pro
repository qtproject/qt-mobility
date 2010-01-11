TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL

QMAKE_CXXFLAGS+=-Werror

PUBLIC_HEADERS += \
           qsensor.h\
           qsensorfactory.h\
           qorientationsensor.h\
           qaccelerometer.h\
           qrotationsensor.h\
           qproximitysensor.h\
           qcompass.h\
           qambientlightsensor.h\
           qtapsensor.h\
           qsensormanager.h\
           qsensorbackend.h\
           qsensorplugin.h\

PRIVATE_HEADERS += \
           qsensorpluginloader_p.h\

SOURCES += qsensor.cpp \
           qsensorfactory.cpp\
           qorientationsensor.cpp\
           qaccelerometer.cpp\
           qrotationsensor.cpp\
           qproximitysensor.cpp\
           qcompass.cpp\
           qambientlightsensor.cpp\
           qtapsensor.cpp\
           qsensormanager.cpp\
           qsensorbackend.cpp\
           qsensorplugin.cpp\
           qsensorpluginloader.cpp\

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

include(../../features/deploy.pri)
