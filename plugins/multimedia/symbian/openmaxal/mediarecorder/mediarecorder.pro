TEMPLATE = lib
CONFIG += plugin mobility
TARGET = $$qtLibraryTarget(qtmultimedia_xarecordservice)

PLUGIN_SUBDIR = mediaservice
MOBILITY = multimedia

include (../../../../../common.pri)

# Symbian specific definition
# ---------------------------
symbian {

# Input parameters for qmake to make the dll a qt plugin
# ------------------------------------------------------
pluginstub.sources = qmakepluginstubs/qtmultimedia_xarecordservice.dll
pluginstub.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
DEPLOYMENT += pluginstub

# Input parameters for the generated bld.inf file
# -----------------------------------------------
SYMBIAN_PLATFORMS = DEFAULT

# Input parameters for the generated mmp file
# -------------------------------------------
TARGET.UID3 = 0x10207CA1
TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCALLOWDLLDATA = 1

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += /epoc32/include/platform/mw/khronos

# Macros controlling debug traces
#DEFINES += PROFILE_TIME
#DEFINES += PROFILE_RAM_USAGE
#DEFINES += PROFILE_HEAP_USAGE
#DEFINES += PLUGIN_QT_TRACE_ENABLED
#DEFINES += PLUGIN_QT_SIGNAL_EMIT_TRACE_ENABLED
#DEFINES += PLUGIN_SYMBIAN_TRACE_ENABLED
}


DEPENDPATH += .

# Input
HEADERS += \
    qxarecordmediaserviceproviderplugin.h \
    qxarecordmediaservice.h \
    qxarecordsession.h \
    qxaaudioendpointselector.h \
    qxaaudioencodercontrol.h \
    qxamediacontainercontrol.h \
    qxamediarecordercontrol.h \
    xarecordsessionimpl.h \
    xarecordsessioncommon.h \
    qxacommon.h

SOURCES += \
    qxarecordmediaserviceproviderplugin.cpp \
    qxarecordmediaservice.cpp \
    qxarecordsession.cpp \
    qxaaudioendpointselector.cpp \
    qxaaudioencodercontrol.cpp \
    qxamediacontainercontrol.cpp \
    qxamediarecordercontrol.cpp \
    xarecordsessionimpl.cpp

LIBS += \
    -lQtMultimedia \
    -lopenmaxal \
    -lbafl

# check for PROFILE_RAM_USAGE
contains(DEFINES, PROFILE_RAM_USAGE) {
    LIBS += -lhal
}
