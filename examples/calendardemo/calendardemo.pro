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
contains(mobility_modules,versit) {
    MOBILITY += versit
    DEFINES += BUILD_VERSIT
}

# Input
SOURCES += src/main.cpp \
           src/calendardemo.cpp \
           src/monthpage.cpp \
           src/daypage.cpp \
           src/eventeditpage.cpp \
           src/todoeditpage.cpp \
           src/journaleditpage.cpp \
           src/eventoccurrenceeditpage.cpp \
           src/addcalendarpage.cpp \
           src/editcalendarspage.cpp
           
HEADERS += src/calendardemo.h \
           src/monthpage.h \
           src/daypage.h \
           src/eventeditpage.h \
           src/todoeditpage.h \
           src/journaleditpage.h \
           src/eventoccurrenceeditpage.h \
           src/addcalendarpage.h \
           src/editcalendarspage.h

INCLUDEPATH += . \
    ../../include \
    ../../src/organizer/ \
    ../../src/organizer/items \
    ../../src/organizer/details \
    ../../src/organizer/filters \
    ../../src/organizer/requests \
    ../../src/versit \
    ../../src/versitorganizer


symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData
}
