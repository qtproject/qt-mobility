TEMPLATE = app
TARGET = servicefw
CONFIG += console mobility
MOBILITY = serviceframework

INCLUDEPATH += ../../src/serviceframework

SOURCES = servicefw.cpp


maemo5|maemo6{
    HEADERS += ../../src/serviceframework/servicemetadata_p.h
    SOURCES += ../../src/serviceframework/servicemetadata.cpp
}

include(../../common.pri)

include(../../features/deploy.pri)
