# #####################################################################
# Versit
# #####################################################################
TEMPLATE = lib
TARGET = QtVersit
include(../../common.pri)

DEFINES += QT_BUILD_VERSIT_LIB QT_MAKEDLL QT_ASCII_CAST_WARNINGS

CONFIG += mobility
MOBILITY = contacts organizer

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
    qversitcontactexporter.h \
    qversitcontactimporter.h \
    qversitorganizerexporter.h \
    qversitorganizerimporter.h \
    qversitresourcehandler.h

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
    qversitorganizerdefs_p.h \
    qversitcontacthandler.h \
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
    qversitcontactexporter.cpp \
    qversitcontactexporter_p.cpp \
    qversitcontactimporter.cpp \
    qversitcontactimporter_p.cpp \
    qversitorganizerexporter.cpp \
    qversitorganizerexporter_p.cpp \
    qversitorganizerimporter.cpp \
    qversitorganizerimporter_p.cpp \
    qvcardbackuphandlers_p.cpp \
    qversitresourcehandler.cpp \
    qversitcontacthandler.cpp \
    qversitpluginloader_p.cpp \
    versitutils.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian { 
    TARGET.UID3 = 0x2002BFBF
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
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
