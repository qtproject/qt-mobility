# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer

QT += network
include (../../common.pri)

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

PUBLIC_HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h

HEADERS += $$PUBLIC_HEADERS

include($$PWD/bearer.pri)
