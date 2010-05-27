# -------------------------------------------------
# Project created by QtCreator 2010-04-22T13:08:00
# -------------------------------------------------
QT += core
TARGET = messagingconsole
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
include(../../common.pri)
include(../../examples/examples.pri)

# Build against the messaging library
INCLUDEPATH += ../../src/messaging
CONFIG += mobility
MOBILITY = messaging
QMAKE_CXXFLAGS += -DUSE_TABBED_LAYOUT
QMAKE_RPATHDIR += ../../lib
LIBS += -lcommhistory
SOURCES += main.cpp \
    testsms.cpp
HEADERS += testsms.h
