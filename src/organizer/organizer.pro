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
PUBLIC_HEADERS += \
    qorganizercollection.h \
    qorganizercollectionchangeset.h \
    qorganizercollectionenginelocalid.h \
    qorganizercollectionid.h \
    qorganizeritemabstractrequest.h \
    qorganizeritemchangeset.h \
    qorganizeritemdetail.h \
    qorganizeritemdetaildefinition.h \
    qorganizeritemdetailfielddefinition.h \
    qorganizeritemfetchhint.h \
    qorganizeritemfilter.h \
    qorganizeritem.h \
    qorganizeritemid.h \
    qorganizeritemenginelocalid.h \
    qorganizeritemmanager.h \
    qorganizeritemmanagerengine.h \
    qorganizeritemmanagerenginefactory.h \
    qorganizeritemrecurrencerule.h \
    qorganizeritemsortorder.h \
    qtorganizerglobal.h \
    qtorganizer.h

PRIVATE_HEADERS += \
    qorganizercollection_p.h \
    qorganizercollectionchangeset_p.h \
    qorganizercollectionid_p.h \
    qorganizeritemabstractrequest_p.h \
    qorganizeritemchangeset_p.h \
    qorganizeritem_p.h \
    qorganizeritemdetail_p.h \
    qorganizeritemid_p.h \
    qorganizeritemfilter_p.h \
    qorganizeritemfetchhint_p.h \
    qorganizeritemdetailfielddefinition_p.h \
    qorganizeritemdetaildefinition_p.h \
    qorganizeritemmanager_p.h \
    qorganizeritemrecurrencerule_p.h \
    qorganizeritemsortorder_p.h

SOURCES += \
    qorganizercollection.cpp \
    qorganizercollectionchangeset.cpp \
    qorganizercollectionenginelocalid.cpp \
    qorganizercollectionid.cpp \
    qorganizeritemabstractrequest.cpp \
    qorganizeritemchangeset.cpp \
    qorganizeritem.cpp \
    qorganizeritemdetail.cpp \
    qorganizeritemdetaildefinition.cpp \
    qorganizeritemdetailfielddefinition.cpp \
    qorganizeritemfetchhint.cpp \
    qorganizeritemfilter.cpp \
    qorganizeritemid.cpp \
    qorganizeritemenginelocalid.cpp \
    qorganizeritemmanager.cpp \
    qorganizeritemmanagerengine.cpp \
    qorganizeritemmanagerenginefactory.cpp \
    qorganizeritemrecurrencerule.cpp \
    qorganizeritemsortorder.cpp \
    qorganizeritemmanager_p.cpp

HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

maemo5 {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=maemo5
}

maemo6 {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=maemo6
}

symbian {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=symbian

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    TARGET.UID3 = 0x2002BFCD
    
    LIBS += -lefsrv

    # ## Organizer
    # Main library
    ORGANIZER_DEPLOYMENT.sources = QtOrganizer.dll
    ORGANIZER_DEPLOYMENT.path = /sys/bin
    DEPLOYMENT += ORGANIZER_DEPLOYMENT
}

CONFIG += app
maemo5 {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=maemo5
}

!isEmpty(ORGANIZER_DEFAULT_ENGINE): DEFINES += Q_ORGANIZER_DEFAULT_ENGINE=$$ORGANIZER_DEFAULT_ENGINE

include(../../features/deploy.pri)
