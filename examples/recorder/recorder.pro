TEMPLATE = app
CONFIG += example

include(../../common.pri)

LIBS += -lQtMedia

HEADERS = recorder.h

SOURCES = \
    main.cpp \
    recorder.cpp

FORMS += recorder.ui
