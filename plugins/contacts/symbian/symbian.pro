######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobapicontactspluginsymbian)
include(../../../common.pri)
symbian: { 
	load(data_caging_paths)

	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
  TARGET.UID3 = 0xE5E2F4B4
  
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  
  INCLUDEPATH += inc
	INCLUDEPATH += rss
	INCLUDEPATH += $$SOURCE_DIR/contacts 
	INCLUDEPATH += $$SOURCE_DIR/contacts/details 
	INCLUDEPATH += $$SOURCE_DIR/contacts/filters 
	INCLUDEPATH += $$SOURCE_DIR/contacts/requests

	
  HEADERS += \
        $$PUBLIC_HEADERS \
        inc/cntsymbianengine.h \
        inc/cntsymbianengine_p.h \
        inc/cnttransformcontact.h \
        inc/cnttransformcontactdata.h \
        inc/cnttransformname.h \
        inc/cnttransformnickname.h \
        inc/cnttransformphonenumber.h \
        inc/cnttransformemail.h \
        inc/cnttransformaddress.h \
        inc/cnttransformurl.h \
        inc/cnttransformbirthday.h \
        inc/cnttransformonlineaccount.h \
        inc/cnttransformorganisation.h \
        inc/cnttransformavatar.h \
        inc/cnttransformavatarsimple.h \
        inc/cnttransformsynctarget.h \
        inc/cnttransformgender.h \
        inc/cnttransformanniversary.h \
        inc/cnttransformanniversarysimple.h \
        inc/cnttransformgeolocation.h \
        inc/cnttransformnote.h \
        inc/cnttransformfamily.h \  
        inc/cntabstractcontactfilter.h \
        inc/cntsymbianfilterdbms.h \
        inc/cntsymbianfiltersql.h \
        inc/cntabstractcontactsorter.h \
        inc/cntsymbiansorterdbms.h \
        inc/cntrelationship.h \
        inc/cntabstractrelationship.h \
        inc/cntrelationshipgroup.h \
        inc/cntsymbianfiltersqlhelper.h \
        inc/cntsymbiansrvconnection.h \
        inc/cntsymbiantransformerror.h \
        inc/cntsymbiandatabase.h
      
  SOURCES += \
        src/cntsymbianengine.cpp \
        src/cntsymbianengine_p.cpp \
        src/cnttransformcontact.cpp \
        src/cnttransformcontactdata.cpp \
        src/cnttransformname.cpp \
        src/cnttransformnickname.cpp \
        src/cnttransformphonenumber.cpp \
        src/cnttransformemail.cpp \
        src/cnttransformaddress.cpp \
        src/cnttransformurl.cpp \
        src/cnttransformbirthday.cpp \
        src/cnttransformonlineaccount.cpp \
        src/cnttransformorganisation.cpp \
        src/cnttransformavatar.cpp \
        src/cnttransformavatarsimple.cpp \
        src/cnttransformsynctarget.cpp \
        src/cnttransformgender.cpp \
        src/cnttransformanniversary.cpp \
        src/cnttransformanniversarysimple.cpp \
        src/cnttransformgeolocation.cpp \
        src/cnttransformnote.cpp \
        src/cnttransformfamily.cpp \  
        src/cntsymbianfilterdbms.cpp \
        src/cntsymbianfiltersql.cpp \
        src/cntsymbiansorterdbms.cpp \
        src/cntrelationship.cpp \
        src/cntabstractrelationship.cpp \
        src/cntrelationshipgroup.cpp \
        src/cntsymbianfiltersqlhelper.cpp \
        src/cntsymbiansrvconnection.cpp \
        src/cntsymbiantransformerror.cpp \
        src/cntsymbiandatabase.cpp
      
  LIBS += \
  				-lcntmodel \
  				-lcentralrepository \
  				-lQtContacts \
  				-lestor \

	target.path = /sys/bin
	INSTALLS += target

    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        DEFINES += USE_CUSTOM_CNT_MODEL_FIELDS
        cntmodelResourceFile = \
            "START RESOURCE ../rss/cntmodel.rss" \
            "TARGETPATH $${CONTACTS_RESOURCE_DIR}" \
            "END"
        MMP_RULES += cntmodelResourceFile
    }

 
	symbianplugin.sources = $${TARGET}.dll
	symbianplugin.path = /resource/qt/plugins/contacts
	DEPLOYMENT += symbianplugin
}
