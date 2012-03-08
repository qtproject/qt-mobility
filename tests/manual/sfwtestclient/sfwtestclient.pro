TEMPLATE = app
TARGET = sfwtestclient 

QT        += core \
    gui 

CONFIG += mobility
MOBILITY = serviceframework

HEADERS   += sfwtestclient.h
SOURCES   += main.cpp \
    sfwtestclient.cpp
FORMS	  += sfwtestclient.ui

symbian: {
	TARGET.UID3 = 0xe0d26e2c 
}


