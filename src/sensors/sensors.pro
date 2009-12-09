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
           qsensormanager.h\

PRIVATE_HEADERS += \

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qsensor.cpp \
           qsensorfactory.cpp\
           qorientationsensor.cpp\
           qaccelerationsensor.cpp\
           qrotationsensor.cpp\
           qproximitysensor.cpp\
           qsensormanager.cpp\

include(../../features/deploy.pri)
