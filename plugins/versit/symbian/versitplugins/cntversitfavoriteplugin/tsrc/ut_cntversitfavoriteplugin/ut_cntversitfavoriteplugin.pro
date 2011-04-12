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
    #HEADERS += ut_cntversitfavoriteplugin.loc
    
    HEADERS += ut_cntversitfavoriteplugin.h
    HEADERS += ../../inc/cntversitfavoritepluginfactory.h
    HEADERS += ../../inc/cntversitfavoriteplugin.h
    SOURCES += ut_cntversitfavoriteplugin.cpp
    SOURCES += ../../src/cntversitfavoritepluginfactory.cpp
    SOURCES += ../../src/cntversitfavoriteplugin.cpp
    TARGET.CAPABILITY = ALL \
        -TCB
        
    CONFIG += symbian_test
}
