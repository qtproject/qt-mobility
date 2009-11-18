TEMPLATE = lib
CONFIG += plugin
QT += phonon
TARGET = $$qtLibraryTarget(phononengine)

PLUGIN_TYPE = mediaservice

include (../../../common.pri)

qtAddLibrary(QtMedia)

INCLUDEPATH += ../../../src/multimedia

DEPENDPATH += .

HEADERS += \
    qphononplayercontrol.h \
    qphononplayerservice.h \
    qphononvideowidget.h \
    qphononserviceplugin.h \
    qphononmetadataprovider.h \

SOURCES += \
    qphononplayercontrol.cpp \
    qphononplayerservice.cpp \
    qphononvideowidget.cpp \
    qphononserviceplugin.cpp \
    qphononmetadataprovider.cpp \

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}
target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
