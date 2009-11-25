INCLUDEPATH += $$PWD

symbian:LIBS += -lmediaclientaudio \
    -lmediaclientaudioinputstream

HEADERS += $$PWD/s60audioencodercontrol.h \
    $$PWD/s60audiomediarecordercontrol.h \
    $$PWD/s60audiodevicecontrol.h \
    $$PWD/s60audiocaptureservice.h \
    $$PWD/s60audiocapturesession.h

SOURCES += $$PWD/s60audioencodercontrol.cpp \
    $$PWD/s60audiomediarecordercontrol.cpp \
    $$PWD/s60audiodevicecontrol.cpp \
    $$PWD/s60audiocaptureservice.cpp \
    $$PWD/s60audiocapturesession.cpp
