TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

#INCLUDEPATH += .

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL

QMAKE_CXXFLAGS+=-Werror

PUBLIC_HEADERS += \
           qsensor.h\
           qsensorfactory.h\
           qorientationsensor.h\
           qaccelerationsensor.h\
           qrotationsensor.h\
           qproximitysensor.h\
           qnorthsensor.h\
           qsensormanager.h\
           qsensorbackend.h\

PRIVATE_HEADERS += \

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qsensor.cpp \
           qsensorfactory.cpp\
           qorientationsensor.cpp\
           qaccelerationsensor.cpp\
           qrotationsensor.cpp\
           qproximitysensor.cpp\
           qnorthsensor.cpp\
           qsensormanager.cpp\
           qsensorbackend.cpp\

CONFIG+=n900
n900 {
    HEADERS += n900filebasedsensor.h
    SOURCES += n900accelerationsensor.cpp\
               #n900lightsensor.cpp\
               n900proximitysensor.cpp\

}

include(../../features/deploy.pri)
