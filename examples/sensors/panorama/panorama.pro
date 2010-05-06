include(../sensors.pri)
QT += gui \
    core
CONFIG += mobility
MOBILITY += sensors
HEADERS += inputcontroller.h \
    keycontroller.h \
    timedcontroller.h \
    view.h \
    accelerometercontroller.h \
    compasscontroller.h \
    magnetometercontroller.h \
    orientationcontroller.h \
    rotationcontroller.h \
    tapcontroller.h
SOURCES += main.cpp \
    inputcontroller.cpp \
    keycontroller.cpp \
    timedcontroller.cpp \
    view.cpp \
    accelerometercontroller.cpp \
    compasscontroller.cpp \
    magnetometercontroller.cpp \
    orientationcontroller.cpp \
    rotationcontroller.cpp \
    tapcontroller.cpp
RESOURCES += panorama.qrc
TARGET = panorama
