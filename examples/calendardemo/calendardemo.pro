######################################################################
#
# Simple example of how to use the organizer API
#
######################################################################

TEMPLATE = app
TARGET = calendardemo
include(../examples.pri)
DEPENDPATH += .
CONFIG += mobility
MOBILITY = organizer

# Input
SOURCES += src/main.cpp \
           src/calendardemo.cpp \
           src/monthpage.cpp \
           src/daypage.cpp \
           src/eventeditpage.cpp
           
HEADERS += src/calendardemo.h \
           src/monthpage.h \
           src/daypage.h \
           src/eventeditpage.h
           
symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
