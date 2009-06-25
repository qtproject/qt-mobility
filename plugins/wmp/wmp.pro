TEMPLATE = lib
TARGET = qwmp
CONFIG  += dll warn_on

DESTDIR = ../bin
!static:DEFINES += QT_MAKEDLL

DEFINES += QT_BUILD_WMP_LIB

DESTDIR = $$OUT_PWD/../../lib

LIBS += -levr -lstrmiids

HEADERS = \
    qevrwidget.h \
    qmfactivate.h \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpmetadata.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h

SOURCES = \
    qevrwidget.cpp \
    qmfactivate.cpp \
    qwmpevents.cpp \
    qwmpmetadata.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp
