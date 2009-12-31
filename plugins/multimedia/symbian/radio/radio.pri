INCLUDEPATH += $$PWD

exists($${EPOCROOT}epoc32\release\armv5\lib\tunerutility.lib) {
	CONFIG += TUNERLIB
	LIBS += -ltunerutility
	LIBS += -lAudioOutputRouting
	DEFINES += TUNERLIBUSED
	INCLUDEPATH += $${EPOCROOT}epoc32\include\mmf\common
	
} else {
       warning("Radio isn't compiled in 3.1 due to missing tuner libraries.")
}

exists($${EPOCROOT}epoc32\release\armv5\lib\Radio_Utility.lib) {
   	CONFIG += RADIOUTILITYLIB
	LIBS += -lRadio_Utility
	DEFINES += RADIOUTILITYLIBUSED
} else {
       warning("Radio isn't compiled in 3.2 -> due to missing radio utility libraries.")
}

TUNERLIB {
HEADERS += \
    $$PWD/s60radiotunerservice.h \
    $$PWD/s60radiotunercontrol_31.h
SOURCES += \
    $$PWD/s60radiotunerservice.cpp \
    $$PWD/s60radiotunercontrol_31.cpp
}

RADIOUTILITYLIB {
HEADERS += \
    $$PWD/s60radiotunerservice.h \
    $$PWD/s60radiotunercontrol_since32.h
SOURCES += \
    $$PWD/s60radiotunerservice.cpp \
    $$PWD/s60radiotunercontrol_since32.cpp
}