#-------------------------------------------------
#
# Project created by QtCreator 2010-01-21T13:14:55
#
#-------------------------------------------------
REL_PATH_TO_SRC = ./src/

TEMPLATE = lib
TARGET = ichecklib
CONFIG += mobility
DEFINES += ICHECKLIB_LIBRARY ICHECK_BUILD

INCLUDEPATH += ./ \
    $$REL_PATH_TO_SRC/plugins \
    $$REL_PATH_TO_SRC/libs \
    $$REL_PATH_TO_SRC/shared/cplusplus \
    $$REL_PATH_TO_SRC/libs/cplusplus \
    $(QTDIR)

include(../../../common.pri)
include(../../../features/deploy.pri)

#replace the output folder to the bin folder because this
#library isn't a pugin, it will be used  by a tool located in bin.
DESTDIR -= $$OUTPUT_DIR/lib
DESTDIR += $$OUTPUT_DIR/bin

include(ichecklib.pri)
