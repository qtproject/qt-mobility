###############################################################################
# Simple compilation test for the presence of Immersion TouchSense Player 
# support.
###############################################################################

CONFIG -= qt
CONFIG += console
TEMPLATE = app
TARGET = 
DEPENDPATH += 
INCLUDEPATH += .

SOURCES += main.cpp
LIBS += -limmvibe

maemo5 || maemo6 {
    ## should use pkg-config for this, but immvibe doesn't have a .pc yet.
    LIBS += -lrt
}
