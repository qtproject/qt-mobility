include(../../../staticconfig.pri)

INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/effects
INCLUDEPATH += ../../../src/global

TARGET  = declarative_multimedia
TARGETPATH = QtMobility/multimedia
include(../../qimportbase.pri)
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

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir
