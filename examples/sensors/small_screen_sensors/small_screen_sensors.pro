TARGET = SmallSensors
TEMPLATE = app

QT=core gui
CONFIG+=mobility
MOBILITY+=sensors

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

#INCLUDEPATH += ..\..\include

symbian:TARGET.CAPABILITY += ReadDeviceData
