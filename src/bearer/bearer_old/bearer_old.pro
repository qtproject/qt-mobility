# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer{000a0000}

QT += network
message("Building QtBearer 10.0 to support version conflict in Symbian")

include ($$PWD/../../../common.pri) 
include ($$PWD/../bearer.pri)


HEADERS += $$PWD/../qnetworkconfiguration.h \
           $$PWD/../qnetworksession.h \
           $$PWD/../qnetworkconfigmanager.h
         

symbian {
    CONFIG += def_files_disabled
    MMP_RULES += defBlock
    defBlock = \
    "$${LITERAL_HASH}ifdef WINSCW" \
    "DEFFILE ../../s60installs/bwins/QtBearer.def" \
    "$${LITERAL_HASH}elif defined EABI" \
    "DEFFILE ../../s60installs/eabi/QtBearer.def" \
    "$${LITERAL_HASH}endif"
}

VERSION=10.0
