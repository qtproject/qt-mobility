######################################################################
#
# Contacts Mobility API - dummy test plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin testplugin
TARGET = $$qtLibraryTarget(contacts_testdummy)

include(../../../../common.pri)

SOURCES += dummyplugin.cpp

DEFINES += DUMMYPLUGINNAME=testdummy

# We need to link against this for all sorts of reasons :)
LIBS += -lQtContacts

include(../../contacts_plugins.pri)
