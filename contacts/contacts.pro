# #####################################################################
# Contacts Mobility API
# #####################################################################
TEMPLATE = lib
TARGET = $$qtLibraryTarget(QtContacts)
DEFINES += BUILD_QTCONTACTS \
    QT_ASCII_CAST_WARNINGS

include(../common.pri)

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
    qcontactfilter.h \
    qcontactgroup.h \
    qcontactmanager.h \
    qcontactmanagerengine.h \
    qcontactmanagerenginefactory.h \
    qcontactmanagerinfo.h \
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
    qcontactfilter_p.h \
    qcontactgroup_p.h \
    qcontactmanager_p.h \
    qcontacts_p.h \
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
    qcontactfilter.cpp \
    qcontactgroup.cpp \
    qcontactmanager_p.cpp \
    qcontactmanager.cpp \
    qcontactmanagerengine.cpp \
    qcontactmanagerenginefactory.cpp \
    qcontactmanagerinfo.cpp \
    qcontactsortorder.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders

    # This is for new exporting system coming in garden
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
}

