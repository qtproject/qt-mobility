# #####################################################################
# Contacts Mobility API
# #####################################################################
TEMPLATE = lib
TARGET = QtContacts
DEFINES += BUILD_QTCONTACTS \
    QT_ASCII_CAST_WARNINGS

include(../common.pri)

include(details/details.pri)
include(engines/engines.pri)
include(filters/filters.pri)
include(requests/requests.pri)

# Input
HEADERS += qcontact.h \
    qcontact_p.h \
    qcontactabstractaction.h \
    qcontactabstractactionfactory.h \
    qcontactdetail.h \
    qcontactdetail_p.h \
    qcontactdetaildefinition.h \
    qcontactdetaildefinition_p.h \
    qcontactfilter.h \
    qcontactfilter_p.h \
    qcontactgroup.h \
    qcontactgroup_p.h \
    qcontactmanager.h \
    qcontactmanager_p.h \
    qcontactmanagerengine.h \
    qcontactmanagerenginefactory.h \
    qcontactmanagerinfo.h \
    qcontacts_p.h \
    qcontactsortorder.h \
    qcontactsortorder_p.h \
    qtcontactsglobal.h \
    qtcontacts.h

SOURCES += qcontact.cpp \
    qcontactabstractaction.cpp \
    qcontactabstractactionfactory.cpp \
    qcontactdetail.cpp \
    qcontactdetaildefinition.cpp \
    qcontactfilter.cpp \
    qcontactgroup.cpp \
    qcontactmanager_p.cpp \
    qcontactmanager.cpp \
    qcontactmanagerengine.cpp \
    qcontactmanagerenginefactory.cpp \
    qcontactmanagerinfo.cpp \
    qcontactsortorder.cpp
