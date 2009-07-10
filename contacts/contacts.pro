######################################################################
#
# Contacts Mobility API
#
######################################################################

TEMPLATE = lib
TARGET = QtContacts

include(../common.pri)

DEFINES += BUILD_QTCONTACTS

# Input
HEADERS += \
        qcontact.h \
        qcontact_p.h \
        qcontactaddress.h \
        qcontactanniversary.h \
        qcontactavatar.h \
        qcontactbirthday.h \
        qcontactdetail.h \
        qcontactdetail_p.h \
        qcontactdetaildefinition.h \
        qcontactdetaildefinition_p.h \
        qcontactdetails.h \
        qcontactdisplaylabel.h \
        qcontactguid.h \
        qcontactgender.h \
        qcontactgroup.h \
        qcontactgroup_p.h \
        qcontactname.h \
        qcontactorganisation.h \
        qcontactphonenumber.h \
        qcontactemailaddress.h \
        qcontactaddress.h \
        qcontactguid.h \
        qcontactmanager.h \
        qcontactmanager_p.h \
        qcontactmanagerinfo.h \
        qcontacts_p.h \
        qcontactsynctarget.h \
        qcontacturl.h \
        qtcontactsglobal.h \
        qtcontacts.h


SOURCES += \
        qcontact.cpp \
        qcontactdetail.cpp \
        qcontactdetaildefinition.cpp \
        qcontactdetails.cpp \
        qcontactgroup.cpp \
        qcontactmanager_p.cpp \
        qcontactmanager.cpp \
        qcontactmanagerinfo.cpp

# invalid backend (nonoptional)
HEADERS += ./invalid/qcontactinvalidbackend_p.h
SOURCES += ./invalid/qcontactinvalidbackend.cpp

# in memory backend
contains(CONTACTS_BACKEND, memory) {
HEADERS += ./memory/qcontactmemorybackend_p.h
SOURCES += ./memory/qcontactmemorybackend.cpp
}

