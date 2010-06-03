###############################################################################
# Simple compilation test for the presence of One Click Connectivity
# support. In practice it is supported Symbian^3 onwards.
###############################################################################

CONFIG -= qt
TEMPLATE = app
TARGET = 
DEPENDPATH += 
INCLUDEPATH += .
INCLUDEPATH += \epoc32\include\app
INCLUDEPATH += \epoc32\include\platform\app
INCLUDEPATH += \epoc32\include\app\loc 

SOURCES += main.cpp

    LIBS += -lcommdb \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -linsock \
            -lecom \
            -lefsrv \
            -lextendedconnpref \
            -lnetmeta
