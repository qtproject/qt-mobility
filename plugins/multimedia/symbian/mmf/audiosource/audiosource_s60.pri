INCLUDEPATH += $$PWD

symbian:LIBS += -lmediaclientaudio \
    -lmediaclientaudioinputstream \
    -lmmfcontrollerframework \
    -lcone \ 
    -lbafl

HEADERS += $$PWD/s60audioencodercontrol.h \
    $$PWD/s60audiomediarecordercontrol.h \
    $$PWD/s60audioendpointselector.h \
    $$PWD/s60audiocaptureservice.h \
    $$PWD/s60audiocapturesession.h \
    $$PWD/S60audiocontainercontrol.h

SOURCES += $$PWD/s60audioencodercontrol.cpp \
    $$PWD/s60audiomediarecordercontrol.cpp \
    $$PWD/s60audioendpointselector.cpp \
    $$PWD/s60audiocaptureservice.cpp \
    $$PWD/s60audiocapturesession.cpp \
    $$PWD/S60audiocontainercontrol.cpp
