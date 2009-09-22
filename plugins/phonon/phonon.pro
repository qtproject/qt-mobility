
TEMPLATE = lib
CONFIG += plugin
QT += phonon
TARGET = phononengine

PLUGIN_SUBDIR = mediaservice

include (../../common.pri)

LIBS += -lQtMedia

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
