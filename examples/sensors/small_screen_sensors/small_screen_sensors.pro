TARGET = SmallSensors
TEMPLATE = app

include(../../examples.pri)

QT=core gui
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors

SOURCES += main.cpp \
    sensorswindow.cpp \
    freefallform.cpp \
    accelerationform.cpp \
    orientationform.cpp

HEADERS += sensorswindow.h \
    freefallform.h \
    accelerationform.h \
    orientationform.h

FORMS += sensorswindow.ui \
    freefallform.ui \
    accelerationform.ui \
    orientationform.ui

symbian:TARGET.CAPABILITY += ReadDeviceData
