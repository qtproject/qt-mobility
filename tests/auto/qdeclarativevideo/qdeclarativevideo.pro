TARGET = tst_qdeclarativevideo
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/video
DEPENDPATH += ../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/multimedia

HEADERS += \
        $$PWD/../../../plugins/declarative/multimedia/qdeclarativevideo_p.h \
        $$PWD/../../../plugins/declarative/multimedia/qdeclarativemediabase_p.h \
        $$PWD/../../../plugins/declarative/multimedia/qdeclarativemediametadata_p.h

SOURCES += \
        tst_qdeclarativevideo.cpp \
        $$PWD/../../../plugins/declarative/multimedia/qdeclarativevideo.cpp \
        $$PWD/../../../plugins/declarative/multimedia/qdeclarativemediabase.cpp

CONFIG += mobility
MOBILITY = multimedia

QT += declarative
