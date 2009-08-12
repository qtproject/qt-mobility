TEMPLATE = app
CONFIG += example

contains(QT_CONFIG, multimedia): QT += multimedia

INCLUDEPATH += $$PWD/../../multimedia \
    $$PWD/../../multimedia/endpoints

include(../../common.pri)

LIBS += -lQtMedia

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
