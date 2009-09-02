TEMPLATE = app
CONFIG += example

INCLUDEPATH += $$PWD/../../multimedia

include(../../common.pri)

contains(QT_CONFIG, multimedia) QT += multimedia

LIBS += -lQtMedia

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
