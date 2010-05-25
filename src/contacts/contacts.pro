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

CONFIG += mobility
MOBILITY = serviceframework

# contacts depends on service framework (for actions)
INCLUDEPATH += ../serviceframework

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
    qcontactdetailfielddefinition.h \
    qcontactfetchhint.h \
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
    qcontactactionservicemanager_p.h \
    qcontactchangeset_p.h \
    qcontactdetail_p.h \
    qcontactdetaildefinition_p.h \
    qcontactdetailfielddefinition_p.h \
    qcontactfetchhint_p.h \
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
    qcontactactionservicemanager_p.cpp \
    qcontactchangeset.cpp \
    qcontactdetail.cpp \
    qcontactdetaildefinition.cpp \
    qcontactdetailfielddefinition.cpp \
    qcontactfetchhint.cpp \
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

maemo5 {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=maemo5
}

maemo6 {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=tracker
}

maemo5|maemo6 {
    CONFIG += create_pc create_prl
    QMAKE_PKGCONFIG_DESCRIPTION = Qt Mobility - Contacts API
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtContacts.pc

    INSTALLS += pkgconfig
}

wince* {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=wince
}

symbian {
    isEmpty(CONTACTS_DEFAULT_ENGINE): CONTACTS_DEFAULT_ENGINE=symbian

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002AC7A

    LIBS += -lefsrv

    ### Contacts
    # Main library
    CONTACTS_DEPLOYMENT.sources = QtContacts.dll
    CONTACTS_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += CONTACTS_DEPLOYMENT
}

!isEmpty(CONTACTS_DEFAULT_ENGINE): DEFINES += Q_CONTACTS_DEFAULT_ENGINE=$$CONTACTS_DEFAULT_ENGINE

CONFIG += app
include(../../features/deploy.pri)
