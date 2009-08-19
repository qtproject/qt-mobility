######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_symbians60)

include(../../../common.pri)

HEADERS += qcontactsymbians60backend_p.h
SOURCES += qcontactsymbians60backend.cpp

INCLUDEPATH += .
INCLUDEPATH += $$SOURCE_DIR/contacts 
INCLUDEPATH += $$SOURCE_DIR/contacts/details 
INCLUDEPATH += $$SOURCE_DIR/contacts/filters 
INCLUDEPATH += $$SOURCE_DIR/contacts/requests
    
LIBS += -lQtContacts 

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_GENERAL_DLL

target.path = /sys/bin
INSTALLS += target

testplugin.sources = $${TARGET}.dll
testplugin.path = /resource/qt/plugins/contacts
DEPLOYMENT += testplugin
