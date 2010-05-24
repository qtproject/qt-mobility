TEMPLATE = app
CONFIG += example

INCLUDEPATH += ../../src/multimedia
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    audiorecorder.h
  
SOURCES = \
    main.cpp \
    audiorecorder.cpp

symbian: {
    TARGET.CAPABILITY = UserEnvironment ReadDeviceData
}
