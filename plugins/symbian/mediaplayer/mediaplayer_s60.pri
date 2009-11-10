INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lws32 \
	-lfbscli \
	-lcone \
	-lMPEngine.dll

HEADERS += \
    $$PWD/s60mediaplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60mediaplayersession.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60videometadataprovider.h \
    $$PWD/s60videosurface.h \
    $$PWD/s60videooverlay.h \
    $$PWD/s60videorenderer.h \
    $$PWD/s60mediarecognizer.h \
    $$PWD/s60audioplayersession.h \
    $$PWD/ms60mediaplayerresolver.h

SOURCES += \
    $$PWD/s60mediaplayercontrol.cpp \
    $$PWD/s60mediaplayerservice.cpp \
    $$PWD/s60mediaplayersession.cpp \
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60videometadataprovider.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60mediarecognizer.cpp \
    $$PWD/s60audioplayersession.cpp
	


