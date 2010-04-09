INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lmediaclientaudio \
	-lws32 \
	-lfbscli \
	-lcone \
    -lmmfcontrollerframework \
    -lefsrv \
    -lbitgdi 

# If support to DRM is wanted then comment out the following line 
#CONFIG += drm_supported

drm_supported {
    LIBS + = -ldrmaudioplayutility
    DEFINES += S60_DRM_SUPPORTED
}    

HEADERS += \
    $$PWD/s60mediaplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60mediaplayersession.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60mediametadataprovider.h \
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
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60mediametadataprovider.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60mediarecognizer.cpp \
    $$PWD/s60audioplayersession.cpp \
    $$PWD/s60videowidget.cpp \
    $$PWD/s60mediaplayeraudioendpointselector.cpp

contains(S60_VERSION, 3.1) {
    MMP_RULES += "$${LITERAL_HASH}ifndef WINSCW" \
    "LIBRARY    MPEngine.lib" \
    "MACRO    HAS_MEDIA_PLAYER" \
    "$${LITERAL_HASH}endif"
   
    !exists($${EPOCROOT}epoc32\release\winscw\udeb\audiooutputrouting.lib) {
        MMP_RULES += "$${LITERAL_HASH}ifdef WINSCW" \
                     "MACRO HAS_NO_AUDIOROUTING" \
                     "$${LITERAL_HASH}else" \
                     "LIBRARY audiooutputrouting.lib" \
                     "$${LITERAL_HASH}endif"
        message("Note: AudioOutput Routing API not supported for 3.1 winscw target")
    } else {
        MMP_RULES +="LIBRARY audiooutputrouting.lib"
    }

} else {
    LIBS += -lMPEngine
    DEFINES += HAS_MEDIA_PLAYER
    LIBS += -laudiooutputrouting
}

exists($${EPOCROOT}epoc32\include\platform\mw\mediarecognizer.h) {
    symbian:LIBS += -lplaybackhelper
    DEFINES += USE_SYMBIAN_MEDIARECOGNIZER
    #these are sdk plugins that don't exits in Symbian3
    symbian:LIBS -= -lMPEngine
    message("Using Symbian mediarecognizer")
}
exists($$[QT_INSTALL_HEADERS]\QtGui\private\qwidget_p.h) {
    DEFINES += USE_PRIVATE_QWIDGET_METHODS
    message("Enabling use of private QWidget methods")
}

