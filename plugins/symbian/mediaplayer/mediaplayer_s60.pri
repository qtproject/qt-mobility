INCLUDEPATH += $$PWD
LIBS += -lmediaclientvideo \
	-lws32 \
	-lfbscli \
	-lcone
#ws32.lib & fbscli.lib 
HEADERS += \
    $$PWD/s60videoplayercontrol.h \
    $$PWD/s60mediaplayerservice.h \
    $$PWD/s60videoplayersession.h \
    $$PWD/s60videometadataprovider.h \
	$$PWD/S60videoplayeroutputcontrol.h \
	$$PWD/S60videowidget.h \
    $$PWD/s60videosurface.h \
    $$PWD/s60videooverlay.h \
    $$PWD/s60videorenderer.h \
    $$PWD/s60videorendererinterface.h

SOURCES += \
    $$PWD/s60videoplayercontrol.cpp \
    $$PWD/s60mediaplayerservice.cpp \
    $$PWD/s60videoplayersession.cpp \
    $$PWD/s60videometadataprovider.cpp \
	$$PWD/S60videoplayeroutputcontrol.cpp \
	$$PWD/S60videowidget.cpp \
    $$PWD/s60videosurface.cpp \
    $$PWD/s60videooverlay.cpp \
    $$PWD/s60videorenderer.cpp \
    $$PWD/s60videorendererinterface.cpp



