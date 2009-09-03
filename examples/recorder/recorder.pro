TEMPLATE = app
CONFIG += example

include(../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia

LIBS += -lQtMedia

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
