###############################################################################
# Simple compilation test for the presence of Symbian^4 interim calendar API
# support which includes the TType ENote (Agenda-related)
###############################################################################

CONFIG -= qt
CONFIG += console
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

SOURCES += main.cpp

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
LIBS += -lcalinterimapi
