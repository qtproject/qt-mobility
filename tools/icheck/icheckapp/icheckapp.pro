#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T15:27:35
#
#-------------------------------------------------

TEMPLATE = app
TARGET = icheck
CONFIG   += console mobility

INCLUDEPATH += ../ichecklib

CONFIG(debug, debug|release){
    LIBS += -L../../../Bin -lichecklibd
}
else {
    LIBS += -L../../../Bin -lichecklib
}

include(../../../common.pri)
include(../../../features/deploy.pri)

SOURCES += main.cpp
