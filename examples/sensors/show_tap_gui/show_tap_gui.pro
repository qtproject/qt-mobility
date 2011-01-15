include(../sensors.pri)
QT += core \
    gui
CONFIG += mobility
CONFIG += debug
MOBILITY += sensors
TARGET = show_tap_gui
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    tap.cpp
HEADERS += mainwindow.h \
    tap.h
FORMS += mainwindow.ui
