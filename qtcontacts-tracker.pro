######################################################################
#
# Contacts Mobility API Tracker storage plugin
#
######################################################################

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(contacts_tracker)

HEADERS += qcontacttrackerbackend_p.h
SOURCES += qcontacttrackerbackend.cpp

INCLUDEPATH += ../../qtmobility/contacts

target.path = /usr/lib/qt4/plugins/qtcontacts-tracker
INSTALLS += target
