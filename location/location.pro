# Qt location library
TEMPLATE = lib
TARGET = QtLocation

#DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

include (../common.pri)

HEADERS += draft-api.h

SOURCES += draft-api.cpp
