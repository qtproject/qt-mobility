TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_symbian)
PLUGIN_TYPE=organizer
    
CONFIG += mobility
MOBILITY = organizer

include(../../../common.pri)

INCLUDEPATH += ../../../src/organizer \
                ../../../src/organizer/items \
                ../../../src/organizer/requests \
                ../../../src/organizer/filters \
                ../../../src/organizer/details

symbian: { 
    load(data_caging_paths)

    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002BFC9
  
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    HEADERS += \
        qorganizersymbian_p.h \
        organizeritemtransform.h \
        transform/organizeritemdetailtransform.h \
        transform/organizereventtimerangetransform.h \
        transform/organizeritemdescriptiontransform.h \
        transform/organizeritemdisplaylabeltransform.h \
        transform/organizeritemguidtransform.h \
        transform/organizeritemlocationtransform.h \
        transform/organizeritemprioritytransform.h \
        transform/organizeritemrecurrencetransform.h \
        transform/organizeritemtimestamptransform.h \
        transform/organizeritemtypetransform.h \
        transform/organizerjournaltimerangetransform.h \
        transform/organizertodoprogresstransform.h \
        transform/organizertodotimerangetransform.h
    SOURCES += \
        qorganizersymbian.cpp \
        organizeritemtransform.cpp \
        transform/organizeritemdetailtransform.cpp \
        transform/organizereventtimerangetransform.cpp \
        transform/organizeritemdescriptiontransform.cpp \
        transform/organizeritemdisplaylabeltransform.cpp \
        transform/organizeritemguidtransform.cpp \
        transform/organizeritemlocationtransform.cpp \
        transform/organizeritemprioritytransform.cpp \
        transform/organizeritemrecurrencetransform.cpp \
        transform/organizeritemtimestamptransform.cpp \
        transform/organizeritemtypetransform.cpp \
        transform/organizerjournaltimerangetransform.cpp \
        transform/organizertodoprogresstransform.cpp \
        transform/organizertodotimerangetransform.cpp

    LIBS += -lcalinterimapi

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}
