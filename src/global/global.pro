#for now we don't actually have anything to build
#just ensure installation of public headers
TEMPLATE = subdirs

include(../../common.pri)
PUBLIC_HEADERS += qmobilityglobal.h

headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE
INSTALLS+= headers
CONFIG += middleware
include(../../features/deploy.pri)


