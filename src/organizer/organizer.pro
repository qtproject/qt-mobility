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
    qorganizercollectionengineid.h \
    qorganizercollectionid.h \
    qorganizerabstractrequest.h \
    qorganizeritemchangeset.h \
    qorganizeritemdetail.h \
    qorganizeritemdetaildefinition.h \
    qorganizeritemdetailfielddefinition.h \
    qorganizeritemfetchhint.h \
    qorganizeritemfilter.h \
    qorganizeritem.h \
    qorganizeritemid.h \
    qorganizeritemengineid.h \
    qorganizermanager.h \
    qorganizermanagerengine.h \
    qorganizermanagerenginefactory.h \
    qorganizerrecurrencerule.h \
    qorganizeritemsortorder.h \
    qtorganizerglobal.h \
    qtorganizer.h

PRIVATE_HEADERS += \
    qorganizercollection_p.h \
    qorganizercollectionchangeset_p.h \
    qorganizerabstractrequest_p.h \
    qorganizeritemchangeset_p.h \
    qorganizeritem_p.h \
    qorganizeritemdetail_p.h \
    qorganizeritemfilter_p.h \
    qorganizeritemfetchhint_p.h \
    qorganizeritemdetailfielddefinition_p.h \
    qorganizeritemdetaildefinition_p.h \
    qorganizermanager_p.h \
    qorganizerrecurrencerule_p.h \
    qorganizeritemsortorder_p.h

SOURCES += \
    qorganizercollection.cpp \
    qorganizercollectionchangeset.cpp \
    qorganizercollectionengineid.cpp \
    qorganizercollectionid.cpp \
    qorganizerabstractrequest.cpp \
    qorganizeritemchangeset.cpp \
    qorganizeritem.cpp \
    qorganizeritemdetail.cpp \
    qorganizeritemdetaildefinition.cpp \
    qorganizeritemdetailfielddefinition.cpp \
    qorganizeritemfetchhint.cpp \
    qorganizeritemfilter.cpp \
    qorganizeritemid.cpp \
    qorganizeritemengineid.cpp \
    qorganizermanager.cpp \
    qorganizermanagerengine.cpp \
    qorganizermanagerenginefactory.cpp \
    qorganizerrecurrencerule.cpp \
    qorganizeritemsortorder.cpp \
    qorganizermanager_p.cpp

HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

maemo5 {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=maemo5
}

maemo6 {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=mkcal
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

simulator {
    isEmpty(ORGANIZER_DEFAULT_ENGINE): ORGANIZER_DEFAULT_ENGINE=simulator
}

!isEmpty(ORGANIZER_DEFAULT_ENGINE): DEFINES += Q_ORGANIZER_DEFAULT_ENGINE=$$ORGANIZER_DEFAULT_ENGINE

include(../../features/deploy.pri)
