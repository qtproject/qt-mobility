# #####################################################################
# Versit-Organizer Import/Export
# #####################################################################
TEMPLATE = lib
TARGET = QtVersitOrganizer
include(../../common.pri)

DEFINES += QT_BUILD_VERSIT_ORGANIZER_LIB QT_MAKEDLL QT_ASCII_CAST_WARNINGS

qtAddLibrary(QtVersit)
qtAddLibrary(QtOrganizer)

INCLUDEPATH += . \
               ../versit \
               ../organizer \
               ../organizer/requests \
               ../organizer/filters \
               ../organizer/details \
               ../organizer/items

# Input
PUBLIC_HEADERS +=  \
    qversitorganizerhandler.h \
    qversitorganizerexporter.h \
    qversitorganizerimporter.h \
    qversittimezonehandler.h

# Private Headers
PRIVATE_HEADERS += \
    qversitorganizerexporter_p.h \
    qversitorganizerimporter_p.h \
    qversitorganizerdefs_p.h \
    qversitorganizerpluginloader_p.h \
    timezones_p.h

# Implementation
SOURCES += \
    qversitorganizerexporter.cpp \
    qversitorganizerexporter_p.cpp \
    qversitorganizerimporter.cpp \
    qversitorganizerimporter_p.cpp \
    qversitorganizerhandler.cpp \
    qversitorganizerpluginloader_p.cpp \
    timezones_p.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian { 
    TARGET.UID3 = 0x200315FB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    LIBS += -lefsrv

    VERSIT_ORGANIZER_DEPLOYMENT.sources = QtVersitOrganizer.dll
    VERSIT_ORGANIZER_DEPLOYMENT.path = /sys/bin
    DEPLOYMENT += VERSIT_ORGANIZER_DEPLOYMENT
}

maemo5|maemo6 {
    CONFIG += create_pc create_prl
    QMAKE_PKGCONFIG_DESCRIPTION = Qt Mobility - Versit API for Organizer
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtVersitOrganizer.pc

    INSTALLS += pkgconfig
}

CONFIG += middleware
include(../../features/deploy.pri)
