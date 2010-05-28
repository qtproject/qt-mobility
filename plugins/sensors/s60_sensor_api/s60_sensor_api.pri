INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

HEADERS += qs60sensorapiaccelerometer.h

SOURCES += main.cpp \
           qs60sensorapiaccelerometer.cpp


# Enable this to build winscw and comment LIBS += -lRRSensorApi out
# MMP_RULES does not work with sbsv2
# Sbsv2 does not work with s60 v3.1 winscw (works fine with armv5)
#LIBS += -lRRSensorApi
MMP_RULES += "$${LITERAL_HASH}ifndef WINSCW" \
             "LIBRARY    RRSensorApi.lib" \
             "$${LITERAL_HASH}else" \
             "MACRO HAS_NO_SENSOR_PROVISION" \
             "$${LITERAL_HASH}endif"

message("Note: RRSensor API not supported for 3.1 winscw target")
