TEMPLATE = app
TARGET = 
QT += testlib
CONFIG += qtestlib
include(../tsrc.pri)

DEFINES += PBK_UNIT_TEST
DEPENDPATH += .
INCLUDEPATH += .

symbian:
{
    INCLUDEPATH += $$SYMBIAN_PATHS
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    HEADERS += $$SYMBIAN_HEADERS
    
    SOURCES += $$SYMBIAN_SOURCES \
               ut_cntfilteringdbms.cpp

    CONFIG += mobility
    MOBILITY = contacts

    TARGET.CAPABILITY = ALL -TCB
    
    LIBS += $$SYMBIAN_LIBS
}
