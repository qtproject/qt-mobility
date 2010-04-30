TEMPLATE=app
TARGET=vsexplorer
CONFIG+=console

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

#enable_readline {
#    MODULES*=readline
#    DEFINES+=USE_READLINE
#}

SOURCES=\
    vsexplorer.cpp

include(../../common.pri)

CONFIG += mobility
MOBILITY = publishsubscribe

include(../../features/deploy.pri)
