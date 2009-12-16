TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL

QMAKE_CXXFLAGS+=-Werror

PUBLIC_HEADERS += \
           qsensor.h\
           qsensorfactory.h\
           qorientationsensor.h\
           qaccelerationsensor.h\
           qrotationsensor.h\
           qproximitysensor.h\
           qmagneticnorthsensor.h\
           qambientlightsensor.h\

PRIVATE_HEADERS += \

SOURCES += qsensor.cpp \
           qsensorfactory.cpp\
           qorientationsensor.cpp\
           qaccelerationsensor.cpp\
           qrotationsensor.cpp\
           qproximitysensor.cpp\
           qmagneticnorthsensor.cpp\
           qambientlightsensor.cpp\

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

#include(n900/n900.pri)

include(../../features/deploy.pri)
