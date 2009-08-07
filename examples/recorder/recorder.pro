TEMPLATE = app
CONFIG += example

# Qt += multimedia
# DEFINES += AUDIOSERVICES
include(../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia \
    $$PWD/../../multimedia/endpoints
LIBS += -lQtMedia
QMAKE_RPATHDIR += $$OUT_PWD/../../lib

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
