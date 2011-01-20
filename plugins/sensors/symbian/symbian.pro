INCLUDEPATH+=$$(EPOCROOT)epoc32/include/osextensions

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_sym)

include(version.pri)
include(../../../common.pri)

SOURCES +=  \
			sensorbackendsym.cpp \
			proximitysensorsym.cpp \
			ambientlightsensorsym.cpp \
			magnetometersensorsym.cpp \
			compasssym.cpp \
			accelerometersym.cpp \
			orientationsym.cpp \
			rotationsensorsym.cpp \
			tapsensorsym.cpp \
			main.cpp \
			
HEADERS += \
			sensorbackendsym.h \
			sensorbackenddatasym.h \
			proximitysensorsym.h \
			ambientlightsensorsym.h \
			magnetometersensorsym.h \
			compasssym.h \
			accelerometersym.h \
			orientationsym.h \
			rotationsensorsym.h \
			tapsensorsym.h \
           
equals(sensors_symbian_light_enabled,yes) {
    HEADERS += lightsensorsym.h
    SOURCES += lightsensorsym.cpp
    DEFINES += ENABLE_LIGHT_SENSOR
}

QT=core
CONFIG+=mobility
MOBILITY+=sensors

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002BFC8
    TARGET.CAPABILITY = ALL -TCB    
    LIBS += -lsensrvclient
    LIBS += -lsensrvutil   

    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
}
