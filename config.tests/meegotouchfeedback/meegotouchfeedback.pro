###############################################################################
# Simple compilation test for the presence of the meegotouch haptic theme lib
###############################################################################

CONFIG += qt
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

SOURCES += main.cpp
LIBS += -lmeegotouchcore
