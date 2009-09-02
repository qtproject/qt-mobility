TEMPLATE = app
CONFIG += example

INCLUDEPATH += $$PWD/../../multimedia

include(../../common.pri)

LIBS += -lQtMedia

HEADERS = cameracapture.h

SOURCES = \
    main.cpp \
    cameracapture.cpp

FORMS += cameracapture.ui
