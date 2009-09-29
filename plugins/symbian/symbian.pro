TEMPLATE = lib
CONFIG += plugin
TARGET = QtMobilityMultimediaEngine
PLUGIN_SUBDIR = mediaservice
include (../../common.pri)
LIBS += -lQtMedia
DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia
HEADERS += s60serviceplugin.h \
   
SOURCES += s60serviceplugin.cpp \

include(radio/radio.pri)
include(camera/camera_s60.pri)


load(data_caging_paths)
    pluginDep.sources = QtMobilityMultimediaEngine.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR\mediaservice    
    DEPLOYMENT += pluginDep    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    MMP_RULES += EXPORTUNFROZEN
