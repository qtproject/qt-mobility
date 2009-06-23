TEMPLATE = lib
TARGET = qwmp
CONFIG  += dll warn_on

DESTDIR = ../bin
!static:DEFINES += QT_MAKEDLL

DEFINES += QT_BUILD_WMP_LIB

DESTDIR = $$OUT_PWD/../../lib

HEADERS = \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h

SOURCES = \
    qwmpevents.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp
