TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qt7engine)
QT += opengl multimedia

PLUGIN_TYPE = mediaservice

include (../../../common.pri)

qtAddLibrary(QtMedia)

LIBS += -framework AppKit -framework AudioUnit \
        -framework AudioToolbox -framework CoreAudio \
        -framework QuartzCore -framework QTKit

# The Quicktime framework is only awailable for 32-bit builds, so we
# need to check for this before linking against it.
# QMAKE_MAC_XARCH is not awailable on Tiger, but at the same time,
# we never build for 64-bit architechtures on Tiger either:
contains(QMAKE_MAC_XARCH, no) {
    LIBS += -framework QuickTime
} else {
    LIBS += -Xarch_i386 -framework QuickTime -Xarch_ppc -framework QuickTime
}

INCLUDEPATH += ../../../src/multimedia

DEPENDPATH += .

HEADERS += \
    qt7backend.h \
    qt7playercontrol.h \
    qt7videooutput.h \
    qt7movieviewoutput.h \
    qt7movievideowidget.h \
    qt7movieviewrenderer.h \
    qt7playersession.h \
    qt7playerservice.h \
    qt7serviceplugin.h \    
    qt7movierenderer.h \
    qt7playermetadata.h \
    qcvdisplaylink.h


OBJECTIVE_SOURCES += \
    qt7backend.mm \
    qt7playersession.mm \    
    qt7serviceplugin.mm \
    qt7movieviewoutput.mm \
    qt7movievideowidget.mm \
    qt7movieviewrenderer.mm \
    qt7playermetadata.mm \
    qt7movierenderer.mm \
    qt7playercontrol.mm \
    qt7videooutput.mm \
    qt7playerservice.mm \
    qcvdisplaylink.mm


target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
