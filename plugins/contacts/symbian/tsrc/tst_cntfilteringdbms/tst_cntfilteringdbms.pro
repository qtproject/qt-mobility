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
    INCLUDEPATH += \epoc32\include\app
	INCLUDEPATH += \epoc32\include\platform\app
    
    HEADERS += $$SYMBIAN_HEADERS
    
    SOURCES += $$SYMBIAN_SOURCES \
               tst_cntfilteringdbms.cpp

    CONFIG += mobility
    MOBILITY = contacts

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
    
    LIBS += $$SYMBIAN_LIBS
}
