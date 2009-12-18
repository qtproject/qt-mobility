TEMPLATE = app
TARGET = PSPathMapperServer
QT = core
TARGET.UID3 = 0x2002AC88

SOURCES += pspathmapperservermain.cpp

include(../../../common.pri)


DEPENDPATH += ../xqsettingsmanager_symbian
INCLUDEPATH += ../xqsettingsmanager_symbian
DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
include(../xqsettingsmanager_symbian/settingsmanager.pri)

HEADERS += pathmapper_symbian.h \
    qcrmlparser_p.h

SOURCES += pathmapper_symbian.cpp \
    qcrmlparser.cpp \
    pspathmapperserver.cpp \
    pspathmappersession.cpp \
    
            
TARGET.CAPABILITY = ALL -TCB
