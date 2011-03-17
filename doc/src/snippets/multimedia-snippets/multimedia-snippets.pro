# Doc snippets - compiled for truthiness

TEMPLATE = lib
TARGET = qtmmksnippets
include(../../../../features/basic_examples_setup.pri)

INCLUDEPATH += ../../../../src/global \
               ../../../../src/multimedia \
               ../../../../src/multimedia/audio \
               ../../../../src/multimedia/video \
               ../../../../src/multimedia/effects

CONFIG += mobility console
MOBILITY = multimedia

SOURCES += \
    audio.cpp \
    video.cpp \
    camera.cpp \
    media.cpp
