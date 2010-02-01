INCLUDEPATH += $$PWD

DEFINES += QMEDIA_SYMBIAN_CAMERA

exists($${EPOCROOT}epoc32\include\ecamadvancedsettings.h) {
    symbian:LIBS += -lecamadvsettings
    DEFINES += USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER
    message("Using from s60 3.2 CCameraAdvancedSettings header")  
}
exists($${EPOCROOT}epoc32\include\ecamadvsettings.h) {
    symbian:LIBS += -lecamadvsettings
    DEFINES += USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
    message("Using from s60 5.0 CCameraAdvancedSettings header")  
}

symbian:LIBS += -lcamerawrapper \
    -lfbscli \
    -lmediaclientvideo \
    -lecam
    
HEADERS += $$PWD/s60camerafocuscontrol.h \
    $$PWD/s60cameraexposurecontrol.h \
    $$PWD/s60cameracontrol.h \
    $$PWD/s60mediacontrol.h \
    $$PWD/s60camerasession.h \
    $$PWD/s60mediacontainercontrol.h \
    $$PWD/s60videoencoder.h \
    $$PWD/s60cameraservice.h \
    $$PWD/s60cameraimageprocessingcontrol.h \
    $$PWD/s60cameraimagecapturecontrol.h \
    $$PWD/s60cameravideodevicecontrol.h \
    $$PWD/s60cameraimageencodercontrol.h \
    $$PWD/s60viewfinderwidget.h \
    $$PWD/s60camerasettings.h	
SOURCES += $$PWD/s60camerafocuscontrol.cpp \
    $$PWD/s60cameraexposurecontrol.cpp \
    $$PWD/s60cameracontrol.cpp \
    $$PWD/s60mediacontrol.cpp \
    $$PWD/s60camerasession.cpp \
    $$PWD/s60mediacontainercontrol.cpp \
    $$PWD/s60videoencoder.cpp \
    $$PWD/s60cameraservice.cpp \
    $$PWD/s60cameraimageprocessingcontrol.cpp \
    $$PWD/s60cameraimagecapturecontrol.cpp \
    $$PWD/s60cameravideodevicecontrol.cpp \
    $$PWD/s60cameraimageencodercontrol.cpp \
    $$PWD/s60viewfinderwidget.cpp \
    $$PWD/s60camerasettings.cpp
