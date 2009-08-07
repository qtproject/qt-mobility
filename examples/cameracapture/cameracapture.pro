TEMPLATE = app
CONFIG += example

QT += multimedia
# DEFINES += AUDIOSERVICES
INCLUDEPATH += $$PWD/../../multimedia \
    $$PWD/../../multimedia/endpoints

include(../../common.pri)

LIBS += -lQtMedia

QMAKE_RPATHDIR += $$OUT_PWD/../../lib

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
