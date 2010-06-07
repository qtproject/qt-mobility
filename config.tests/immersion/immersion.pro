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
