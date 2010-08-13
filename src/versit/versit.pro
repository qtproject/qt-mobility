# #####################################################################
# Versit
# #####################################################################
TEMPLATE = lib
TARGET = QtVersit
include(../../common.pri)

DEFINES += QT_BUILD_VERSIT_LIB QT_MAKEDLL QT_ASCII_CAST_WARNINGS

CONFIG += mobility

contains(mobility_modules,contacts) {
    MOBILITY += contacts
}
contains(mobility_modules,organizer) {
    MOBILITY += organizer
}

# Contacts Includepath
INCLUDEPATH += . \
               ../contacts \
               ../contacts/requests \
               ../contacts/filters \
               ../contacts/details \
               ../organizer \
               ../organizer/requests \
               ../organizer/filters \
               ../organizer/details \
               ../organizer/items

# Input
PUBLIC_HEADERS +=  \
    qversitdocument.h \
    qversitproperty.h \
    qversitreader.h \
    qversitwriter.h \
    qversitresourcehandler.h
contains(mobility_modules,contacts) {
    PUBLIC_HEADERS +=  \
        qversitcontactexporter.h \
        qversitcontactimporter.h \
        qversitcontacthandler.h
}
contains(mobility_modules,organizer) {
    PUBLIC_HEADERS +=  \
        qversitorganizerexporter.h \
        qversitorganizerimporter.h
}

# Private Headers
PRIVATE_HEADERS += \
    qvcardbackuphandlers_p.h \
    qversitdocument_p.h \
    qversitdocumentwriter_p.h \
    qversitproperty_p.h \
    qversitreader_p.h \
    qversitwriter_p.h \
    qvcard21writer_p.h \
    qvcard30writer_p.h \
    qversitcontactexporter_p.h \
    qversitcontactimporter_p.h \
    qversitorganizerexporter_p.h \
    qversitorganizerimporter_p.h \
    qversitdefs_p.h \
    qversitcontactsdefs_p.h \
    qversitorganizerdefs_p.h \
    qversitpluginloader_p.h \
    versitutils_p.h

# Implementation
SOURCES += qversitdocument.cpp \
    qversitdocument_p.cpp \
    qversitdocumentwriter_p.cpp \
    qversitproperty.cpp \
    qversitreader.cpp \
    qversitreader_p.cpp \
    qversitwriter.cpp \
    qversitwriter_p.cpp \
    qvcard21writer.cpp \
    qvcard30writer.cpp \
    qversitresourcehandler.cpp \
    qversitpluginloader_p.cpp \
    versitutils.cpp
contains(mobility_modules,contacts) {
    SOURCES += \
        qversitcontactexporter.cpp \
        qversitcontactexporter_p.cpp \
        qversitcontactimporter.cpp \
        qversitcontactimporter_p.cpp \
        qversitcontacthandler.cpp \
        qvcardbackuphandlers_p.cpp
}
contains(mobility_modules,organizer) {
    SOURCES += \
        qversitorganizerexporter.cpp \
        qversitorganizerexporter_p.cpp \
        qversitorganizerimporter.cpp \
        qversitorganizerimporter_p.cpp
}

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian { 
    TARGET.UID3 = 0x2002BFBF
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    LIBS += -lefsrv

    VERSIT_DEPLOYMENT.sources = QtVersit.dll
    VERSIT_DEPLOYMENT.path = /sys/bin
    DEPLOYMENT += VERSIT_DEPLOYMENT
}

maemo5|maemo6 {
    CONFIG += create_pc create_prl
    QMAKE_PKGCONFIG_DESCRIPTION = Qt Mobility - Versit API
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtVersit.pc

    INSTALLS += pkgconfig
}

CONFIG += app
include(../../features/deploy.pri)
