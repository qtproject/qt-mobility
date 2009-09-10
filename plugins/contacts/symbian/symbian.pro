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
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCALLOWDLLDATA = 1
  TARGET.UID3 = 0xE5E2F4B4
  
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  
  INCLUDEPATH += inc
	#INCLUDEPATH += ../
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
      inc/transformemail.h
      
  SOURCES += \
  		src/qcontactsymbianbackend.cpp \
  		src/qcontactsymbianengine_p.cpp \
      src/transformcontact.cpp \
      src/transformcontactdata.cpp \
      src/transformname.cpp \
      src/transformnickname.cpp \
      src/transformphonenumber.cpp \
      src/transformemail.cpp
      
  LIBS += \
  				-lcntmodel \
  				-lQtContacts 

target.path = /sys/bin
INSTALLS += target

symbianplugin.sources = $${TARGET}.dll
symbianplugin.path = /resource/qt/plugins/contacts
DEPLOYMENT += symbianplugin

}
