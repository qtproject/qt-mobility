HEADERS = bearerdialog.h \
          sessionwidget.h

SOURCES = main.cpp \
          bearerdialog.cpp \
          sessionwidget.cpp

FORMS = bearerdialog.ui \
        sessionwidget.ui

TARGET = bearermonitor

QT = core gui network

INCLUDEPATH += ../../bearer

include(../../common.pri)

LIBS += -lQtBearer
win32:!wince*:LIBS += -lWs2_32
wince*:LIBS += -lWs2

CONFIG += console

include(../examples.pri)

