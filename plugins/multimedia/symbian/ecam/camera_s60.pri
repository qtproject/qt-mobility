INCLUDEPATH += $$PWD

#build camera service
DEFINES += QMEDIA_SYMBIAN_CAMERA

    exists($${EPOCROOT}epoc32\include\ecamadvancedsettings.h) {
    MMP_RULES += \
    "$${LITERAL_HASH}ifndef WINSCW" \
    "LIBRARY ecamadvsettings.lib" \
    "MACRO USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER" \
    "$${LITERAL_HASH}endif"
    message("Using from s60 3.2 CCameraAdvancedSettings header")
    }
    exists($${EPOCROOT}epoc32\include\ecamadvsettings.h) {
        symbian:LIBS += -lecamadvsettings
        DEFINES += USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
        message("Using from s60 5.0 CCameraAdvancedSettings header")
    }
    exists($${EPOCROOT}epoc32\include\mmf\devvideo\devvideoconstants.h) {
        symbian:LIBS += -ldevvideo
        DEFINES += S60_DEVVIDEO_RECORDING_SUPPORTED
        message("Devvideo API supported")
    }

#3.1 platform uses old style autofocusing
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
        -lcone


    HEADERS += $$PWD/s60camerafocuscontrol.h \
        $$PWD/s60cameraexposurecontrol.h \
        $$PWD/s60cameracontrol.h \
        $$PWD/s60mediacontrol.h \
        $$PWD/s60videocapturesession.h \
        $$PWD/s60imagecapturesession.h \
        $$PWD/s60mediacontainercontrol.h \
        $$PWD/s60videoencoder.h \
        $$PWD/s60audioencoder.h \
        $$PWD/s60cameraservice.h \
        $$PWD/s60cameraimageprocessingcontrol.h \
        $$PWD/s60cameraimagecapturecontrol.h \
        $$PWD/s60cameravideodevicecontrol.h \
        $$PWD/s60cameraimageencodercontrol.h \
        $$PWD/s60viewfinderwidget.h \
    $$PWD/s60camerasettings.h \
    $$PWD/s60cameraengine.h	\
    $$PWD/s60cameraengineobserver.h
    SOURCES += $$PWD/s60camerafocuscontrol.cpp \
        $$PWD/s60cameraexposurecontrol.cpp \
        $$PWD/s60cameracontrol.cpp \
        $$PWD/s60mediacontrol.cpp \
        $$PWD/s60videocapturesession.cpp \
        $$PWD/s60imagecapturesession.cpp \
        $$PWD/s60mediacontainercontrol.cpp \
        $$PWD/s60videoencoder.cpp \
        $$PWD/s60audioencoder.cpp \
        $$PWD/s60cameraservice.cpp \
        $$PWD/s60cameraimageprocessingcontrol.cpp \
        $$PWD/s60cameraimagecapturecontrol.cpp \
        $$PWD/s60cameravideodevicecontrol.cpp \
        $$PWD/s60cameraimageencodercontrol.cpp \
        $$PWD/s60viewfinderwidget.cpp \
    $$PWD/s60camerasettings.cpp \
    $$PWD/s60cameraengine.cpp	\

