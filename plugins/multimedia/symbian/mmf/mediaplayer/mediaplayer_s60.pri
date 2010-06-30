INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lmediaclientaudio \
	-lws32 \
	-lfbscli \
	-lcone \
    -lmmfcontrollerframework \
    -lefsrv \
    -lbitgdi \
    -lapgrfx \
    -lapmime \

# If support to DRM is wanted then comment out the following line
#CONFIG += drm_supported

# We are building Symbian backend with media player support
DEFINES += HAS_MEDIA_PLAYER

drm_supported {
    LIBS + = -ldrmaudioplayutility
    DEFINES += S60_DRM_SUPPORTED
}

HEADERS += \
    $$PWD/s60mediaplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60mediaplayersession.h \
    $$PWD/s60mediametadataprovider.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60videosurface.h \
    $$PWD/s60videooverlay.h \
    $$PWD/s60videorenderer.h \
    $$PWD/s60mediarecognizer.h \
    $$PWD/s60audioplayersession.h \
    $$PWD/ms60mediaplayerresolver.h \
    $$PWD/s60videowidget.h \
    $$PWD/s60mediaplayeraudioendpointselector.h

SOURCES += \
    $$PWD/s60mediaplayercontrol.cpp \
    $$PWD/s60mediaplayerservice.cpp \
    $$PWD/s60mediaplayersession.cpp \
    $$PWD/s60mediametadataprovider.cpp \
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60mediarecognizer.cpp \
    $$PWD/s60audioplayersession.cpp \
    $$PWD/s60videowidget.cpp \
    $$PWD/s60mediaplayeraudioendpointselector.cpp


contains(surfaces_s60_enabled,yes) {
    DEFINES += MMF_VIDEO_SURFACES_SUPPORTED
    message("Surfaces_s60 enabled")
}

contains(S60_VERSION, 3.1) {
    #3.1 doesn't provide audio routing in videoplayer
    contains(audiorouting_s60_enabled,yes) {
    MMP_RULES += "$${LITERAL_HASH}ifndef WINSCW" \
                "MACRO HAS_AUDIOROUTING" \
                "LIBRARY audiooutputrouting.lib" \
                "$${LITERAL_HASH}endif"
    message("Note: AudioOutput Routing API not supported for 3.1 winscw target and in videoplayer")
    }

} else {
    contains(audiorouting_s60_enabled,yes) {
        #We use audiooutputrouting.lib for directing audio output to speaker/earspeaker
        DEFINES += HAS_AUDIOROUTING_IN_VIDEOPLAYER
        DEFINES += HAS_AUDIOROUTING
        message("Audiorouting_s60 enabled for post 3.1 sdk")
        LIBS += -laudiooutputrouting
    }

}

exists($$[QT_INSTALL_HEADERS]\QtGui\private\qwidget_p.h) {
    DEFINES += USE_PRIVATE_QWIDGET_METHODS
    message("Enabling use of private QWidget methods")
}