INCLUDEPATH += $$PWD

# Camera Service
DEFINES += QMEDIA_SYMBIAN_CAMERA

#3.1 Platform uses autofocusing from ForumNokia example
contains(S60_VERSION, 3.1) {
    message ("Using s60 3.1 autofocusing")
    MMP_RULES += \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "LIBRARY camautofocus.lib" \
        "$${LITERAL_HASH}else" \
        "STATICLIBRARY camautofocus_s.lib" \
        "$${LITERAL_HASH}endif // WINS" \
        "MACRO S60_CAM_AUTOFOCUS_SUPPORT"
}

# S60 3.2 Platform:
exists($${EPOCROOT}epoc32\include\ecamadvancedsettings.h) {
    MMP_RULES += \
        "$${LITERAL_HASH}ifndef WINSCW" \
        "LIBRARY ecamadvsettings.lib" \
        "MACRO USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER" \
        "$${LITERAL_HASH}endif"
    message("Using from s60 3.2 CCameraAdvancedSettings header")
}

# S60 5.0 Platform:
exists($${EPOCROOT}epoc32\include\ecamadvsettings.h) {
    symbian:LIBS += -lecamadvsettings
    DEFINES += USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
    message("Using CCameraAdvancedSettings header from S60 5.0")
}

# DevVideo API Check:
exists($${EPOCROOT}epoc32\include\mmf\devvideo\devvideoconstants.h) {
    symbian:LIBS += -ldevvideo
    DEFINES += S60_DEVVIDEO_RECORDING_SUPPORTED
    message("Devvideo API supported")
}

# Libraries:
symbian:LIBS += -lfbscli \
        -lmediaclientvideo \
        -lecam \
        -lbafl \
        -lPlatformEnv \
        -lcharconv \
        -lconvnames \
        -lgb2312_shared \
        -ljisx0201 \
        -ljisx0208 \
        -lmmfcontrollerframework \
        -lfbscli \
        -lefsrv \
        -lcone \
        -lws32

# Source:
HEADERS += $$PWD/s60cameraconstants.h \
    $$PWD/s60cameralockscontrol.h \
    $$PWD/s60camerafocuscontrol.h \
    $$PWD/s60cameraexposurecontrol.h \
    $$PWD/s60cameraflashcontrol.h \
    $$PWD/s60cameracontrol.h \
    $$PWD/s60mediarecordercontrol.h \
    $$PWD/s60videocapturesession.h \
    $$PWD/s60imagecapturesession.h \
    $$PWD/s60mediacontainercontrol.h \
    $$PWD/s60videoencodercontrol.h \
    $$PWD/s60audioencodercontrol.h \
    $$PWD/s60cameraservice.h \
    $$PWD/s60cameraimageprocessingcontrol.h \
    $$PWD/s60cameraimagecapturecontrol.h \
    $$PWD/s60videodevicecontrol.h \
    $$PWD/s60imageencodercontrol.h \
    $$PWD/s60videowidgetcontrol.h \
    $$PWD/s60camerasettings.h \
    $$PWD/s60cameraengine.h	\
    $$PWD/s60cameraviewfinderengine.h \
    $$PWD/s60cameraengineobserver.h \
    $$PWD/s60videorenderercontrol.h

SOURCES += $$PWD/s60cameralockscontrol.cpp \
    $$PWD/s60camerafocuscontrol.cpp \
    $$PWD/s60cameraexposurecontrol.cpp \
    $$PWD/s60cameraflashcontrol.cpp \
    $$PWD/s60cameracontrol.cpp \
    $$PWD/s60mediarecordercontrol.cpp \
    $$PWD/s60videocapturesession.cpp \
    $$PWD/s60imagecapturesession.cpp \
    $$PWD/s60mediacontainercontrol.cpp \
    $$PWD/s60videoencodercontrol.cpp \
    $$PWD/s60audioencodercontrol.cpp \
    $$PWD/s60cameraservice.cpp \
    $$PWD/s60cameraimageprocessingcontrol.cpp \
    $$PWD/s60cameraimagecapturecontrol.cpp \
    $$PWD/s60videodevicecontrol.cpp \
    $$PWD/s60imageencodercontrol.cpp \
    $$PWD/s60videowidgetcontrol.cpp \
    $$PWD/s60camerasettings.cpp \
    $$PWD/s60cameraengine.cpp \
    $$PWD/s60cameraviewfinderengine.cpp \
    $$PWD/s60videorenderercontrol.cpp

# End of file
