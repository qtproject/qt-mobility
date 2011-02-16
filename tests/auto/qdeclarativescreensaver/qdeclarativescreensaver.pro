TARGET = tst_qdeclarativescreensaver
CONFIG+=testcase
QT = gui core network
QT += declarative

INCLUDEPATH += ../../../src/systeminfo
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/systeminfo

SOURCES  += tst_qdeclarativescreensaver.cpp
HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativescreensaver_p.h

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativescreensaver.cpp

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}

