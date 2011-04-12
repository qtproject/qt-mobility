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
    HEADERS += ut_cntversitprefplugin.loc
    
    HEADERS += ut_cntversitprefplugin.h
    HEADERS += ../../inc/cntversitprefpluginfactory.h
    HEADERS += ../../inc/cntversitprefplugin.h
    SOURCES += ut_cntversitprefplugin.cpp
    SOURCES += ../../src/cntversitprefpluginfactory.cpp
    SOURCES += ../../src/cntversitprefplugin.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
        
    CONFIG += symbian_test
}


