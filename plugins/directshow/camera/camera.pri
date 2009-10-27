INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/dscameraservice.h \
    $$PWD/dscameracontrol.h \
    $$PWD/dsvideorenderer.h \
    $$PWD/dsvideooutputcontrol.h \
    $$PWD/dsvideodevicecontrol.h \
    $$PWD/dsimagecapturecontrol.h \
    $$PWD/dscamerasession.h

SOURCES += \
    $$PWD/dscameraservice.cpp \
    $$PWD/dscameracontrol.cpp \
    $$PWD/dsvideorenderer.cpp \
    $$PWD/dsvideooutputcontrol.cpp \
    $$PWD/dsvideodevicecontrol.cpp \
    $$PWD/dsimagecapturecontrol.cpp \
    $$PWD/dscamerasession.cpp

INCLUDEPATH += $(DXSDK_DIR)/include
LIBPATH += $(DXSDK_DIR)/lib -lstrmiids -lole32

