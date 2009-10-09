TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobapicontactspluginsymbian)
PLUGIN_TYPE=contacts

include(../../../common.pri)

symbian: { 
    load(data_caging_paths)

    TARGET.CAPABILITY = CAP_GENERAL_DLL
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
        inc/transformaddress.h

    SOURCES += \
        src/qcontactsymbianbackend.cpp \
        src/qcontactsymbianengine_p.cpp \
        src/transformcontact.cpp \
        src/transformcontactdata.cpp \
        src/transformname.cpp \
        src/transformnickname.cpp \
        src/transformphonenumber.cpp \
        src/transformemail.cpp \
        src/transformaddress.cpp

    qtAddLibbray(QtContacts)      

    LIBS += -lcntmodel

    target.path = /sys/bin
    INSTALLS += target


    cntmodelResourceFile = \
      "START RESOURCE ../rss/cntmodel.rss" \
      "TARGETPATH CONTACTS_RESOURCE_DIR" \
      "END"
    MMP_RULES += cntmodelResourceFile   
  
    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin
}
target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
