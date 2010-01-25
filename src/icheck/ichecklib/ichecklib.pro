#-------------------------------------------------
#
# Project created by QtCreator 2010-01-21T13:14:55
#
#-------------------------------------------------
REL_PATH_TO_SRC = ./src/

QT       -= gui

TARGET = ichecklib
TEMPLATE = lib

DEFINES += ICHECKLIB_LIBRARY ICHECK_BUILD

CONFIG(debug, debug|release){
    TARGET = ichecklibd
    DESTDIR =  ./debug
    DLLDESTDIR = ../icheckapp/debug ../icheckapp/debug

    INCLUDEPATH += ./ \
        ./debug/GeneratedFiles \
        $$REL_PATH_TO_SRC/plugins \
        $$REL_PATH_TO_SRC/libs \
        $$REL_PATH_TO_SRC/shared/cplusplus \
        $$REL_PATH_TO_SRC/libs/cplusplus \
        $(QTDIR)
}
else{
    TARGET = ichecklib
    DESTDIR = ./release
    DLLDESTDIR = ../icheckapp/release
    INCLUDEPATH += ./ \
        ./release/GeneratedFiles \
        $$REL_PATH_TO_SRC/plugins \
        $$REL_PATH_TO_SRC/libs \
        $$REL_PATH_TO_SRC/shared/cplusplus \
        $$REL_PATH_TO_SRC/libs/cplusplus \
        $(QTDIR)
}

include(ichecklib.pri)
