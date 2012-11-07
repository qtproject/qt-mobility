include(../../../features/utils.pri)

TEMPLATE = lib
CONFIG += plugin
QT += core
TARGET = $$mobilityPluginTarget(qtlandmarks_symbian)
PLUGIN_TYPE=landmarks
CONFIG += mobility
MOBILITY = location
include(../../../common.pri)

include(symbian_landmarks_defines.pri)

symbian {

        load(data_caging_paths)
        INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
        TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = $$mobilityUID(0x20031E8F)

    LIBS += \
        -lflogger \
        -leposlandmarks \
        -leposlmmultidbsearch \
        -leposlmsearchlib \
        -leposlmdbmanlib \
        -llbs \
        -leuser \
        -lefsrv \
        -lcone \
        -lbafl \
                -lapgrfx \
                -lefsrv \
                -lapmime

    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += pluginDep
}

HEADERS += 	inc/qlandmarkmanagerenginefactory_symbian.h \
                        inc/qlandmarkmanagerengine_symbian.h \
                        inc/qlandmarkmanagerengine_symbian_p.h \
                        inc/qlandmarkutility.h \
                        inc/qlandmarkdbeventhandler.h \
                        inc/qlandmarkrequesthandler.h

SOURCES += 	src/qlandmarkmanagerenginefactory_symbian.cpp \
                        src/qlandmarkmanagerengine_symbian.cpp \
                        src/qlandmarkmanagerengine_symbian_p.cpp \
                        src/qlandmarkutility.cpp \
                        src/qlandmarkdbeventhandler.cpp \
                        src/qlandmarkrequesthandler.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/landmarks
INSTALLS += target

