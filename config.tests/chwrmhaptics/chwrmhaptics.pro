###############################################################################
# Simple compilation test for the presence of CHWRMHaptics class
###############################################################################

CONFIG -= qt
TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

SOURCES += main.cpp
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
LIBS += -lhwrmhapticsclient

