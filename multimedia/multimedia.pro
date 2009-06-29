TEMPLATE = lib

# distinct from QtMultimedia
TARGET = QtMedia

DESTDIR = $$OUT_PWD/../lib

HEADERS = \
    qabstractmediacontrol.h \
    qabstractmediaobject.h \
    qabstractmediaobject_p.h \
    qabstractmediaservice.h \
    qlocalmediaplaylistsource.h \
    qmediaimageviewer.h \
    qmediametadata.h \
    qmediaplayer.h \
    qmediaplayercontrol.h \
    qmediaplayerservice.h \
    qmediaplaylist.h \
    qmediaplaylistsource.h \
    qmediaproviderfactory_p.h \
    qmediarecorder.h \
    qmediarecordercontrol.h \
    qmediarecorderservice.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmediasink.h \
    qmediasource.h \
    qradioplayer.h \
    qradioservice.h \
    qradiotuner.h \
    qsharedmediaplaylist.h

SOURCES = \
    qabstractmediacontrol.cpp \
    qabstractmediaobject.cpp \
    qabstractmediaservice.cpp \
    qlocalmediaplaylistsource.cpp \
    qmediaimageviewer.cpp \
    qmediametadata.cpp \
    qmediaplayer.cpp \
    qmediaplayercontrol.cpp \
    qmediaplayerservice.cpp \
    qmediaplaylist.cpp \
    qmediaplaylistsource.cpp \
    qmediaproviderfactory.cpp \
    qmediarecorder.cpp \
    qmediarecordercontrol.cpp \
    qmediarecorderservice.cpp \
    qmediaserviceprovider.cpp \
    qmediasource.cpp \
    qradioplayer.cpp \
    qradioservice.cpp \
    qradiotuner.cpp \
    qsharedmediaplaylist.cpp

include (outputs/outputs.pri)


