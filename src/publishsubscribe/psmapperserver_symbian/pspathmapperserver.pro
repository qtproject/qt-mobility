TEMPLATE = app
TARGET = qpspathmapperserver
QT = core
TARGET.UID3 = 0x2002AC88

CONFIG += no_icon

SOURCES += pspathmapperservermain.cpp

include(../../../common.pri)


DEPENDPATH += ../xqsettingsmanager_symbian
INCLUDEPATH += ../xqsettingsmanager_symbian\
               $${EPOCROOT}epoc32\include\platform

DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
DEFINES += XQSETTINGSMANAGER_NO_TRANSACTIONS
DEFINES += XQSETTINGSMANAGER_NO_CENREPKEY_CREATION_DELETION
include(../xqsettingsmanager_symbian/settingsmanager.pri)

HEADERS += pathmapper_symbian_p.h \
    qcrmlparser_p.h

SOURCES += pathmapper_symbian.cpp \
    qcrmlparser.cpp \
    pspathmapperserver.cpp \
    pspathmappersession.cpp \
    
LIBS += -lefsrv

#The ProtServ capability is needed for the server to reside in 
#protected namespace (server name starting with exclamation mark).
TARGET.CAPABILITY = ProtServ
