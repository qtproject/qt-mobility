HEADERS += bbsensorbackend.h \
    bbaccelerometer.h \
    bbaltimeter.h \
    bbambientlightsensor.h \
    bbcompass.h \
    bbgyroscope.h \
    bbirproximitysensor.h \
    bblightsensor.h \
    bbmagnetometer.h \
    bborientationsensor.h \
    bbpressuresensor.h \
    bbproximitysensor.h \
    bbrotationsensor.h \
    bbtemperaturesensor.h \
    bbguihelper.h \
    bbutil.h

SOURCES += bbsensorbackend.cpp \
    bbaccelerometer.cpp \
    bbaltimeter.cpp \
    bbambientlightsensor.cpp \
    bbcompass.cpp \
    bbgyroscope.cpp \
    bbirproximitysensor.cpp \
    bblightsensor.cpp \
    bbmagnetometer.cpp \
    bborientationsensor.cpp \
    bbpressuresensor.cpp \
    bbproximitysensor.cpp \
    bbrotationsensor.cpp \
    bbtemperaturesensor.cpp \
    bbguihelper.cpp \
    bbutil.cpp \
    main.cpp

contains(bbsensor_holster_enabled, yes) {
    HEADERS += bbholstersensor.h
    SOURCES += bbholstersensor.cpp
}
