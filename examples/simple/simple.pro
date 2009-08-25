TEMPLATE=app
INCLUDEPATH += ../../location

HEADERS = locationreceiver.h
SOURCES = locationreceiver.cpp \
          main.cpp

CONFIG += console

include(../../common.pri)
LIBS += -lQtLocation
