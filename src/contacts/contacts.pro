# #####################################################################
# Contacts Mobility API
# #####################################################################
TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtContacts
DEFINES += QT_BUILD_CONTACTS_LIB QT_MAKEDLL\
    QT_ASCII_CAST_WARNINGS

include(../../common.pri)

include(details/details.pri)
include(engines/engines.pri)
include(filters/filters.pri)
include(requests/requests.pri)

# Input
PUBLIC_HEADERS += \
    qcontact.h \
    qcontactabstractrequest.h \
    qcontactaction.h \
    qcontactactiondescriptor.h \
    qcontactactionfactory.h \
    qcontactchangeset.h \
    qcontactdetail.h \
    qcontactdetaildefinition.h \
    qcontactdetaildefinitionfield.h \
    qcontactfilter.h \
    qcontactid.h \
    qcontactmanager.h \
    qcontactmanagerengine.h \
    qcontactmanagerenginefactory.h \
    qcontactrelationship.h \
    qcontactsortorder.h \
    qtcontactsglobal.h \
    qtcontacts.h

# Private Headers
PRIVATE_HEADERS += \
    qcontact_p.h \
    qcontactabstractrequest_p.h \
    qcontactactiondescriptor_p.h \
    qcontactchangeset_p.h \
    qcontactdetail_p.h \
    qcontactdetaildefinition_p.h \
    qcontactdetaildefinitionfield_p.h \
    qcontactfilter_p.h \
    qcontactid_p.h \
    qcontactmanager_p.h \
    qcontactrelationship_p.h \
    qcontactsortorder_p.h

SOURCES += \
    qcontact.cpp \
    qcontactabstractrequest.cpp \
    qcontactaction.cpp \
    qcontactactiondescriptor.cpp \
    qcontactactionfactory.cpp \
    qcontactchangeset.cpp \
    qcontactdetail.cpp \
    qcontactdetaildefinition.cpp \
    qcontactdetaildefinitionfield.cpp \
    qcontactfilter.cpp \
    qcontactid.cpp \
    qcontactmanager_p.cpp \
    qcontactmanager.cpp \
    qcontactmanagerengine.cpp \
    qcontactmanagerenginefactory.cpp \
    qcontactrelationship.cpp \
    qcontactsortorder.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

maemo {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=tracker
}

symbian {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=symbian

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC7A

    ### Contacts
    # Main library
    CONTACTS_DEPLOYMENT.sources = QtContacts.dll
    CONTACTS_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += CONTACTS_DEPLOYMENT

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include/app

    #export headers into EPOCROOT
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
}

!isEmpty(CONTACTS_DEFAULT_ENGINE): DEFINES += Q_CONTACTS_DEFAULT_ENGINE=$$CONTACTS_ENGINE

include(../../features/deploy.pri)

