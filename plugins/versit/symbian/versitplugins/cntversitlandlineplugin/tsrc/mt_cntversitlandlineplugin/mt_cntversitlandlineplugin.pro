TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
QT += testlib \
    sql \
    xml
CONFIG += qtestlib

# Input
HEADERS += mt_cntversitlandlineplugin.h 
SOURCES += mt_cntversitlandlineplugin.cpp 

symbian:
 { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lqtcontacts \
        -lqtversit
        
    CONFIG += symbian_test
}
