TEMPLATE = lib
TARGET = QtSensors

include(../../common.pri)

INCLUDEPATH += .

#Input

DEFINES += QT_BUILD_SENSORS_LIB QT_MAKEDLL

PUBLIC_HEADERS += \
           qsensor.h

PRIVATE_HEADERS += \

SOURCES += qsensor.cpp \

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

include(../../features/deploy.pri)
