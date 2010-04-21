# #####################################################################
# Contacts Mobility API
# #####################################################################
TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtCalendar
DEFINES += QT_BUILD_CALENDAR_LIB \
    QT_MAKEDLL \
    QT_ASCII_CAST_WARNINGS
include(../../common.pri)
include(details/details.pri)
include(engines/engines.pri)
include(filters/filters.pri)
include(requests/requests.pri)

# Input
PUBLIC_HEADERS += qorganizeritemabstractrequest.h \
    qorganizeritemaction.h \
    qorganizeritemactiondescriptor.h \
    qorganizeritemactionfactory.h \
    qorganizeritemchangeset.h \
    qorganizeritemdetail.h \
    qorganizeritemdetaildefinition.h \
    qorganizeritemdetailfielddefinition.h \
    qorganizeritemfetchhint.h \
    qorganizeritemfilter.h \
    qorganizeritem.h \
    qorganizeritemid.h \
    qorganizeritemmanager.h \
    qorganizeritemmanagerengine.h \
    qorganizeritemmanagerenginefactory.h \
    qorganizeritemrecurrencerulecriteria.h \
    qorganizeritemrecurrencerules.h \
    qorganizeritemrelationship.h \
    qorganizeritemsortorder.h \
    qtorganizeritemsglobal.h \
    qtorganizeritem.h

# PRIVATE_HEADERS += \
# SOURCES += \
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS
symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID3 = 0x2002AC7A
    
    # ## Contacts
    # Main library
    CALENDAR_DEPLOYMENT.sources = QtCalendar.dll
    CALENDAR_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += CALENDAR_DEPLOYMENT
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    
    # export headers into EPOCROOT
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
}
include(../../features/deploy.pri)
