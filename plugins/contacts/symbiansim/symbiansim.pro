######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobapicontactspluginsymbiansim)
include(../../../common.pri)
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

    HEADERS += \
        $$PUBLIC_HEADERS \
        inc/cntsymbiansimengine.h
      
    SOURCES += \
        src/cntsymbiansimengine.cpp

    CONFIG += mobility
    MOBILITY = contacts

    LIBS += -lcntmodel \
            -lflogger \
            -lefsrv

    # Uncomment this to use Etel test server (instead of real Etel APIs)
    #DEFINES += SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER

    # add either real or test libraries for Etel
    contains(DEFINES, SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER): {
        message("Using Etel Test Server (not real Etel)")
        INCLUDEPATH +=$${EPOCROOT}epoc32/include/internal
        LIBS += -leteltestserverclient
    } else {
        message("Using real Etel APIs")
        LIBS += -letel \
                -letelmm
    }

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin

}

