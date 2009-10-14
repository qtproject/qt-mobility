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
  		inc/qcontactsymbianbackend.h \
      inc/qcontactsymbianengine_p.h \
      inc/transformcontact.h \
      inc/transformcontactdata.h \
      inc/transformname.h \
      inc/transformnickname.h \
      inc/transformphonenumber.h \
      inc/transformemail.h \
      inc/transformaddress.h \
      inc/transformurl.h \
      inc/transformbirthday.h \
      inc/transformonlineaccount.h \
      inc/transformorganisation.h \
      inc/transformavatar.h \
      inc/transformsynctarget.h \
      inc/transformgender.h \
      inc/transformanniversary.h \
      inc/transformgeolocation.h \
      inc/transformnote.h \  
      inc/qabstractcontactfilter.h \
      inc/qcontactsymbianfilterdbms.h \
      inc/qcontactsymbianfiltersql.h
      
  SOURCES += \
  		src/qcontactsymbianbackend.cpp \
  		src/qcontactsymbianengine_p.cpp \
      src/transformcontact.cpp \
      src/transformcontactdata.cpp \
      src/transformname.cpp \
      src/transformnickname.cpp \
      src/transformphonenumber.cpp \
      src/transformemail.cpp \
      src/transformaddress.cpp \
      src/transformurl.cpp \
      src/transformbirthday.cpp \
      src/transformonlineaccount.cpp \
      src/transformorganisation.cpp \
      src/transformavatar.cpp \
      src/transformsynctarget.cpp \
      src/transformgender.cpp \
      src/transformanniversary.cpp \
      src/transformgeolocation.cpp \
      src/transformnote.cpp \  
      src/qcontactsymbianfilterdbms.cpp \
      src/qcontactsymbianfiltersql.cpp
      
  LIBS += \
  				-lcntmodel \
  				-lcentralrepository \
  				-lQtContacts 

	target.path = /sys/bin
	INSTALLS += target

    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        DEFINES += USE_CUSTOM_CNT_MODEL_FIELDS
        cntmodelResourceFile = \
            "START RESOURCE ../rss/cntmodel.rss" \
            "TARGETPATH CONTACTS_RESOURCE_DIR" \
            "END"
        MMP_RULES += cntmodelResourceFile
    }

 
	symbianplugin.sources = $${TARGET}.dll
	symbianplugin.path = /resource/qt/plugins/contacts
	DEPLOYMENT += symbianplugin
}
