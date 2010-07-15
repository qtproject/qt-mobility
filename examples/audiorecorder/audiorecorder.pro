TEMPLATE = app
CONFIG += example

INCLUDEPATH += ../../src/multimedia ../../src/multimedia/audio
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    audiorecorder.h
  
SOURCES = \
    main.cpp \
    audiorecorder.cpp

maemo*: {
    FORMS += audiorecorder_small.ui
} else {
    FORMS += audiorecorder.ui
}

symbian: {
    TARGET.CAPABILITY = UserEnvironment ReadDeviceData WriteDeviceData 
}
