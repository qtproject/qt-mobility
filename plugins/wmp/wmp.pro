TEMPLATE = lib
TARGET = qwmp
CONFIG  += dll warn_on

DESTDIR = ../bin
!static:DEFINES += QT_MAKEDLL

DEFINES += QT_BUILD_WMP_LIB

DESTDIR = $$OUT_PWD/../../lib

HEADERS = \
    qwmpglobal.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h

SOURCES = \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp
