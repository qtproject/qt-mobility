INCLUDEPATH += $$PWD

exists($${EPOCROOT}epoc32\release\armv5\lib\tunerutility.lib) {
	CONFIG += TUNERLIB
	LIBS += -ltunerutility
	DEFINES += TUNERLIBUSED
	INCLUDEPATH += $${EPOCROOT}epoc32\include\mmf\common	
}

exists($${EPOCROOT}epoc32\release\armv5\lib\Radio_Utility.lib) {
   	CONFIG += RADIOUTILITYLIB
	LIBS += -lRadio_Utility
	DEFINES += RADIOUTILITYLIBUSED
}
contains(QT_CONFIG, TUNERLIB) && !contains(QT_CONFIG, RADIOUTILITYLIB) {
	warning("Radio isn't compiled in due to missing libraries. (3.1 tuner and since 3.2 radio utility libraries)")
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