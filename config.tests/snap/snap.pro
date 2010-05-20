###############################################################################
# Simple compilation test for the presence of Service Network Access Point 
# support. In practice it is supported S60 3rd Edition Featurepack 2 onwards.
###############################################################################

CONFIG -= qt
TEMPLATE = app
TARGET = 
DEPENDPATH += 
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SOURCES += main.cpp
LIBS += -lcmmanager
