######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobapicontactspluginsymbiansim)

include(../../../common.pri)
include(symbiansim_defines.pri)

symbian: { 
    load(data_caging_paths)
    
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002AC85

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    INCLUDEPATH += inc
    INCLUDEPATH += rss
    INCLUDEPATH += $$SOURCE_DIR/contacts
    INCLUDEPATH += $$SOURCE_DIR/contacts/details
    INCLUDEPATH += $$SOURCE_DIR/contacts/filters
    INCLUDEPATH += $$SOURCE_DIR/contacts/requests
    HEADERS += $$PUBLIC_HEADERS \
        inc/cntsymbiansimengine.h \
        inc/cntsymbiansimtransformerror.h \
        inc/cntsimstore.h \
        inc/cntsimstoreprivate.h \
        inc/cntsimstoreeventlistener.h \
        inc/cntabstractsimrequest.h \
        inc/cntsimcontactfetchrequest.h \
        inc/cntsimcontactlocalidfetchrequest.h \
        inc/cntsimcontactremoverequest.h \
        inc/cntsimcontactsaverequest.h \
        inc/cntsimdetaildefinitionfetchrequest.h
        
    SOURCES += src/cntsymbiansimengine.cpp \
        src/cntsymbiansimtransformerror.cpp \
        src/cntsimstore.cpp \
        src/cntsimstoreprivate.cpp \
        src/cntsimstoreeventlistener.cpp \
        src/cntabstractsimrequest.cpp \
        src/cntsimcontactfetchrequest.cpp \
        src/cntsimcontactlocalidfetchrequest.cpp \
        src/cntsimcontactremoverequest.cpp \
        src/cntsimcontactsaverequest.cpp \
        src/cntsimdetaildefinitionfetchrequest.cpp
        
    CONFIG += mobility
    MOBILITY = contacts

    LIBS += -lcntmodel \
            -lflogger \
            -lefsrv

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin

}

