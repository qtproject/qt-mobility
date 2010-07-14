INCLUDEPATH += ../../../src/multimedia \
               ../../../src/multimedia/effects \
               ../../../src/multimedia/video
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_multimedia)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = Qt/multimedia
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += \
        qdeclarativeaudio_p.h \
        qdeclarativemediabase_p.h \
        qdeclarativevideo_p.h \
        qmetadatacontrolmetaobject_p.h \

SOURCES += \
        multimedia.cpp \
        qdeclarativeaudio.cpp \
        qdeclarativemediabase.cpp \
        qdeclarativevideo.cpp \
        qmetadatacontrolmetaobject.cpp

CONFIG += mobility
MOBILITY += multimedia

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

symbian {
    TARGET.UID3 = 0x20021313
    TARGET.EPOCALLOWDLLDATA=1
}
