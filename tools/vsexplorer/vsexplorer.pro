TEMPLATE=app
TARGET=vsexplorer
CONFIG+=console

INCLUDEPATH += ../../context

#enable_readline {
#    MODULES*=readline
#    DEFINES+=USE_READLINE
#}

SOURCES=\
    vsexplorer.cpp

include(../../common.pri)

qtAddLibrary(QtContextFramework)

include(../../features/deploy.pri)
