TEMPLATE = app
TARGET = 
QT += testlib \
    sql \
    xml
CONFIG += qtestlib
CONFIG += mobility
MOBILITY = versit \
    contacts
DEPENDPATH += .
DEPENDPATH += ../..
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += .

symbian:
 { 
    MMP_RULES += "USERINCLUDE ../.."
    
    # Input
    HEADERS += ut_cntversitmycardplugin.loc
    
    HEADERS += ut_cntversitmycardplugin.h
    HEADERS += ../../inc/cntversitmycardpluginfactory.h
    HEADERS += ../../inc/cntversitmycardplugin.h
    SOURCES += ut_cntversitmycardplugin.cpp
    SOURCES += ../../src/cntversitmycardpluginfactory.cpp
    SOURCES += ../../src/cntversitmycardplugin.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
        
    CONFIG += symbian_test
}
