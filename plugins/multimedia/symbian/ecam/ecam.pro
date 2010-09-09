TEMPLATE =      lib
CONFIG +=       plugin

QT +=           multimedia # Really needed?

TARGET =        $$qtLibraryTarget(qtmultimediakit_ecamengine)
PLUGIN_TYPE =   mediaservice
include (../../../../common.pri)

qtAddLibrary(QtMultimediaKit)

CONFIG +=       mobility
MOBILITY +=     multimedia

# Include here so that all defines are added here also
include(camera_s60.pri)

DEPENDPATH += .

INCLUDEPATH += . \
    $${SOURCE_DIR}/include \
    $${SOURCE_DIR}/src/multimedia \
    $${SOURCE_DIR}/src/multimedia/audio \
    $${SOURCE_DIR}/src/multimedia/video \
    $${SOURCE_DIR}

HEADERS += s60cameraserviceplugin.h
SOURCES += s60cameraserviceplugin.cpp

contains(S60_VERSION, 3.1) | contains(S60_VERSION, 3.2) {
    DEFINES += PRE_S60_50_PLATFORM
}

exists($${EPOCROOT}epoc32\\include\\mmf\\common\\mmfvideoenums.h) {
    DEFINES += USE_SYMBIAN_VIDEOENUMS
    message("Using Symbian mmmfvideodeoenums")
}

exists($${EPOCROOT}epoc32\\include\\platform\\mmf\\common\\mmfvideoenums.h) {
    DEFINES += USE_SYMBIAN_VIDEOENUMS
    message("Using Symbian mmmfvideodeoenums")
}

exists($${EPOCROOT}epoc32\\include\\platform\\graphics\\surface.h) {
    DEFINES += SYMBIAN_3_PLATFORM
    MMP_RULES += \
        "MACRO SYMBIAN_3_PLATFORM"
    message("Symbian3: Using directscreen viewfinder")
}

load(data_caging_paths)
TARGET.EPOCALLOWDLLDATA =   1
TARGET.UID3 =               0x2002BFC2
TARGET.CAPABILITY =         ALL -TCB
MMP_RULES +=                EXPORTUNFROZEN # TODO: Remove when stable

# Make a sis package from plugin + api + stub (plugin)
pluginDep.sources = $${TARGET}.dll
pluginDep.path =    $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
DEPLOYMENT +=       pluginDep
