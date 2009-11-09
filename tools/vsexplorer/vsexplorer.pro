TEMPLATE=app
TARGET=vsexplorer
CONFIG+=console

INCLUDEPATH += ../../context
DEPENDPATH += ../../context

#enable_readline {
#    MODULES*=readline
#    DEFINES+=USE_READLINE
#}

SOURCES=\
    vsexplorer.cpp

include(../../common.pri)

qtAddLibrary(QtPublishSubscribe)

include(../../features/deploy.pri)
