#-------------------------------------------------
#
# Project created by QtCreator 2010-01-18T15:27:35
#
#-------------------------------------------------

REL_PATH_TO_SRC = ../../tools/icheck/parser/src

TEMPLATE = app
TARGET = icheck
CONFIG   += console
DEFINES += ICHECK_BUILD ICHECK_APP_BUILD

INCLUDEPATH += . \
    $$REL_PATH_TO_SRC/../ \
    $$REL_PATH_TO_SRC/global \
    $$REL_PATH_TO_SRC/plugins \
    $$REL_PATH_TO_SRC/libs \
    $$REL_PATH_TO_SRC/shared/cplusplus \
    $$REL_PATH_TO_SRC/libs/cplusplus

include(../../common.pri)
include(../../features/deploy.pri)
include(../../tools/icheck/parser/ichecklib.pri)

SOURCES += main.cpp \
        ichecklib.cpp \
        parsemanager.cpp

HEADERS += ichecklib.h \
        ichecklib_global.h \
        parsemanager.h
