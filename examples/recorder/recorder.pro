TEMPLATE = app
CONFIG += example

include(../../common.pri)

contains(QT_CONFIG, multimedia): QT += multimedia

INCLUDEPATH += $$PWD/../../multimedia

LIBS += -lQtMedia

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
