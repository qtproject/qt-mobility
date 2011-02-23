# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer{000a0000}

QT += network
message("Building QtBearer 10.0 to support version conflict in Symbian")

VPATH += ..

HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h

include (../../../common.pri) 
include (../bearer.pri)

symbian {
    
    # there are same name public headers from Qt
    # and this needs to be added to ensure QtBearer 
    # uses its own when the 10.0 version is compiled
    MMP_RULES += "USERINCLUDE .."
    
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
