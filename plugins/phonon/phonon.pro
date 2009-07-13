
TEMPLATE = lib
CONFIG += plugin
QT += phonon
TARGET = phononengine

LIBS += -L../../lib -lQtMedia

DESTDIR = $$OUT_PWD/../../lib/mediaservice
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia \
    ../../multimedia/endpoints

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
