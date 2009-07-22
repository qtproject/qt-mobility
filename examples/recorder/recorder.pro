TEMPLATE = app

# Qt += multimedia
# DEFINES += AUDIOSERVICES
INCLUDEPATH += $$PWD/../../multimedia \
    $$PWD/../../multimedia/endpoints
LIBS += -L$$OUT_PWD/../../lib \
    -lQtMedia
QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
