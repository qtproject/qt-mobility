TEMPLATE = app

# Qt += multimedia
# DEFINES += AUDIOSERVICES
INCLUDEPATH += $$PWD/../../multimedia \
    $$PWD/../../multimedia/endpoints
LIBS += -L$$OUT_PWD/../../lib \
    -lQtMedia
QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
