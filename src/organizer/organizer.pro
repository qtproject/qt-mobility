# #####################################################################
# Organizer Mobility API
# #####################################################################
TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtOrganizer
DEFINES += QT_BUILD_ORGANIZER_LIB \
    QT_MAKEDLL \
    QT_ASCII_CAST_WARNINGS
include(../../common.pri)
include(details/details.pri)
include(items/items.pri)
include(requests/requests.pri)
include(filters/filters.pri)
include(engines/engines.pri)

# Input
PUBLIC_HEADERS += qorganizeritemabstractrequest.h \
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
    qorganizeritemsortorder.h \
    qorganizeritemrecurrence.h \
    qtorganizerglobal.h \
    qtorganizer.h
PRIVATE_HEADERS += qorganizeritemmanager_p.h \
    qorganizeritemabstractrequest_p.h \
    qorganizeritemchangeset_p.h \
    qorganizeritem_p.h \
    qorganizeritemdetail_p.h \
    qorganizeritemid_p.h \
    qorganizeritemfilter_p.h \
    qorganizeritemfetchhint_p.h \
    qorganizeritemdetailfielddefinition_p.h \
    qorganizeritemdetaildefinition_p.h \
    qorganizeritemsortorder_p.h
SOURCES += qorganizer.cpp \
    qorganizeritemabstractrequest.cpp \
    qorganizeritemchangeset.cpp \
    qorganizeritem.cpp \
    qorganizeritemdetail.cpp \
    qorganizeritemdetaildefinition.cpp \
    qorganizeritemdetailfielddefinition.cpp \
    qorganizeritemfetchhint.cpp \
    qorganizeritemfilter.cpp \
    qorganizeritemid.cpp \
    qorganizeritemrecurrence.cpp \
    qorganizeritemmanager.cpp \
    qorganizeritemmanagerengine.cpp \
    qorganizeritemmanagerenginefactory.cpp \
    qorganizeritemsortorder.cpp \
    qorganizeritemmanager_p.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS
symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID3 = 0x2002AC7A
    
    # ## Organizer
    # Main library
    ORGANIZER_DEPLOYMENT.sources = QtOrganizer.dll
    ORGANIZER_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += ORGANIZER_DEPLOYMENT
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    
    # export headers into EPOCROOT
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
}
include(../../features/deploy.pri)
