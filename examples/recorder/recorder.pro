TEMPLATE = app
CONFIG += example

include(../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia

contains(QT_CONFIG, multimedia) QT += multimedia

LIBS += -lQtMedia

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
