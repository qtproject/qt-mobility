#for now we don't actually have anything to build
#just ensure installation of public headers
TEMPLATE = subdirs

include(../../common.pri)

headers.files = qmobilityglobal.h
headers.path = $$QT_MOBILITY_INCLUDE

INSTALLS+= headers


