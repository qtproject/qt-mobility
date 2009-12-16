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

INCLUDEPATH += ../../../src/multimedia

DEPENDPATH += .

HEADERS += \
    qt7backend.h \
    qt7playercontrol.h \
    qt7videooutputcontrol.h \
    qt7movieviewoutput.h \
    qt7movieviewrenderer.h \
    qt7playersession.h \
    qt7playerservice.h \
    qt7serviceplugin.h \

SOURCES += \
    qt7playercontrol.cpp \
    qt7videooutputcontrol.cpp \
    qt7playerservice.cpp

OBJECTIVE_SOURCES += \
    qt7backend.mm \
    qt7playersession.mm \    
    qt7serviceplugin.mm \
    qt7movieviewoutput.mm \
    qt7movieviewrenderer.mm \


target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS+=target
