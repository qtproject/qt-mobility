#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T15:27:35
#
#-------------------------------------------------

QT       -= gui

TARGET = icheck
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ichecklib
CONFIG(debug, debug|release){
    LIBS += -L../ichecklib/debug -lichecklibd
}
else {
    LIBS += -L../ichecklib/release -lichecklib
}

SOURCES += main.cpp
