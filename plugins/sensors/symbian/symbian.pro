INCLUDEPATH+=../../../src/sensors
INCLUDEPATH+=../../sensors \epoc32\include\osextensions

include(version.pri)
include(symbian.pri)
include(../../../common.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_sym)

SOURCES +=  \
			sensorbackendsym.cpp \
			proximitysensorsym.cpp \
			ambientlightsensorsym.cpp \
			magnetometersensorsym.cpp \
			compasssym.cpp \
			accelerometersym.cpp \
			orientationsym.cpp \
			main.cpp
PRIVATE_HEADERS += \
			sensorbackendsym.h \
			sensorbackenddatasym.h \
			proximitysensorsym.h \
			ambientlightsensorsym.h \
			magnetometersensorsym.h \
			compasssym.h \
			accelerometersym.h \
			orientationsym.h
           
HEADERS = $$PRIVATE_HEADERS

#SYSTEM_INCLUDE += ../../sensors

LIBS += -lqtsensors
QT=core
CONFIG+=mobility
MOBILITY+=sensors
DEFINES+=QT_MAKEDLL

symbian {
    TARGET.CAPABILITY = ALL -TCB    
    LIBS += -lSensrvClient
    LIBS += -lsensrvutil   
}
symbian: {
# Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
load(data_caging_paths)
 
# Defines plugin files into Symbian .pkg package
pluginDep.sources = qtsensors_sym.dll
pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
DEPLOYMENT += pluginDep
}

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target

