CONFIG -= qt
TEMPLATE = app

# Input
SOURCES += main.cpp

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

LIBS += -lnfc -lndef -lndefaccess -lnfcdiscoveryservice -lllcp -lnfctagextension

