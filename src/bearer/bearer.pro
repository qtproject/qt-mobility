# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer

QT += network
include (../../common.pri)

PUBLIC_HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h

HEADERS += $$PUBLIC_HEADERS

include(bearer.pri)
