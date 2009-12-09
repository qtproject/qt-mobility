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
        inc/cntsymbiansimengine.h \
      
    SOURCES += \
        src/cntsymbiansimengine.cpp \
      
    qtAddLibrary(QtContacts)

    LIBS += \
            -lcntmodel \
            -letel \
            -letelmm 

    target.path = /sys/bin
    INSTALLS += target

    symbiansimplugin.sources = $${TARGET}.dll
    symbiansimplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbiansimplugin
}

