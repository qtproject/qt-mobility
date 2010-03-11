INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

HEADERS += qs60sensorapicommon.h \
           qs60sensorapiaccelerometer.h

SOURCES += main.cpp \
           qs60sensorapicommon.cpp \
           qs60sensorapiaccelerometer.cpp

LIBS += -lRRSensorApi

# Enable this to build winscw and comment LIBS += -lRRSensorApi out
# MMP_RULES does not work with sbsv2
# Sbsv2 does not work with s60 v3.1 winscw (works fine with armv5)
#MMP_RULES += "$${LITERAL_HASH}ifndef WINSCW" \
#             "LIBRARY    RRSensorApi.lib" \
#             "MACRO    HAS_SENSOR_PLUGIN" \
#             "$${LITERAL_HASH}endif"
