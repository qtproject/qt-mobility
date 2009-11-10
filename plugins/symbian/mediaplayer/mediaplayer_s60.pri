INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lws32 \
	-lfbscli \
	-lcone \
	-lMPEngine.dll

HEADERS += \
    $$PWD/s60videoplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60videometadataprovider.h \
    $$PWD/s60videosurface.h \
    $$PWD/s60videooverlay.h \
    $$PWD/s60videorenderer.h \
    $$PWD/s60mediarecognizer.h

SOURCES += \
    $$PWD/s60videoplayercontrol.cpp \
    $$PWD/s60mediaplayerservice.cpp \
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60videometadataprovider.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60mediarecognizer.cpp



