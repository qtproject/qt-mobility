INCLUDEPATH+=../../../src/sensors
INCLUDEPATH+=../../sensors \epoc32\include\osextensions

include(version.pri)
#include(symbian.pri)
include(../../../common.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(sensors_sym)

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
			
PRIVATE_HEADERS += \
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
           
HEADERS = $$PRIVATE_HEADERS

#SYSTEM_INCLUDE += ../../sensors

LIBS += -lqtsensors
QT=core
CONFIG+=mobility
MOBILITY+=sensors
DEFINES+=QT_MAKEDLL

#QMAKE_CXXFLAGS+=-Werror

#MOC_DIR = moc/
#OBJECTS_DIR = obj/

#DESTDIR = $$OUTPUT_DIR/bin/examples/sensors
#target.path = $$SOURCE_DIR/plugins/sensors
#INSTALLS += target
symbian {
    TARGET.CAPABILITY = ALL -TCB    
    LIBS += -lSensrvClient
    LIBS += -lsensrvutil   
}
symbian: {
# Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
load(data_caging_paths)
 
# Defines plugin files into Symbian .pkg package
pluginDep.sources = sensors_sym.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR/sensors
DEPLOYMENT += pluginDep
}
 
target.path += $$[QT_INSTALL_PLUGINS]/sensors
INSTALLS += target

