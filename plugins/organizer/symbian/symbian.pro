TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtorganizer_symbian)
PLUGIN_TYPE=organizer

# support for S^4-specific types
contains(symbianenote_enabled,yes) {
    DEFINES += AGENDA_EXT_SUPPORT
}

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
    	qorganizeritemrequestqueue.h \
    	organizeritemrequestserviceprovider.h \
        qorganizersymbian_p.h \
        organizersymbiancollection.h \
        organizersymbianutils.h \
        organizeritemtransform.h \
        resetanddestroy.h \
        transform/organizeritemdetailtransform.h \
        transform/organizereventtimerangetransform.h \
        transform/organizeritemdescriptiontransform.h \
        transform/organizeritemdisplaylabeltransform.h \
        transform/organizeritemguidtransform.h \
        transform/organizeriteminstanceorigintransform.h \
        transform/organizeritemlocationtransform.h \
        transform/organizeritemprioritytransform.h \
        transform/organizeritemrecurrencetransform.h \
        transform/organizeritemtimestamptransform.h \
        transform/organizeritemtypetransform.h \
        transform/organizerjournaltimerangetransform.h \
        transform/organizertodoprogresstransform.h \
        transform/organizertodotimerangetransform.h \
        transform/organizeritemremindertransform.h

    SOURCES += \
    	qorganizeritemrequestqueue.cpp \
    	organizeritemrequestserviceprovider.cpp \
        qorganizersymbian.cpp \
        organizersymbiancollection.cpp \
        organizersymbianutils.cpp \
        organizeritemtransform.cpp \
        transform/organizeritemdetailtransform.cpp \
        transform/organizereventtimerangetransform.cpp \
        transform/organizeritemdescriptiontransform.cpp \
        transform/organizeritemdisplaylabeltransform.cpp \
        transform/organizeritemguidtransform.cpp \
        transform/organizeriteminstanceorigintransform.cpp \
        transform/organizeritemlocationtransform.cpp \
        transform/organizeritemprioritytransform.cpp \
        transform/organizeritemrecurrencetransform.cpp \
        transform/organizeritemtimestamptransform.cpp \
        transform/organizeritemtypetransform.cpp \
        transform/organizerjournaltimerangetransform.cpp \
        transform/organizertodoprogresstransform.cpp \
        transform/organizertodotimerangetransform.cpp \
        transform/organizeritemremindertransform.cpp

    LIBS += -lcalinterimapi

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}
