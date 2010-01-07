# #####################################################################
# Versit
# #####################################################################
TEMPLATE = lib
TARGET = QtVersit
include(../../common.pri)

DEFINES += QT_BUILD_VERSIT_LIB QT_MAKEDLL QT_ASCII_CAST_WARNINGS

CONFIG += mobility
MOBILITY = contacts

# Contacts Includepath
INCLUDEPATH += . \
               ../contacts \
               ../contacts/requests \
               ../contacts/filters \
               ../contacts/details

# Input
PUBLIC_HEADERS +=  \
    qversitdocument.h \
    qversitproperty.h \
    qversitreader.h \
    qversitwriter.h \
    qversitcontactexporter.h \
    qversitcontactimporter.h

# Private Headers
PRIVATE_HEADERS += qversitdocument_p.h \
    qversitproperty_p.h \
    qversitreader_p.h \
    qversitwriter_p.h \
    qvcard21writer_p.h \
    qvcard30writer_p.h \
    qversitcontactexporter_p.h \
    qversitcontactimporter_p.h \
    qversitdefs.h \
    versitutils_p.h

# Implementation
SOURCES += qversitdocument.cpp \
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
    versitutils.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include/mw
    DEPLOYMENT += exportheaders
    
    # This is for new exporting system coming in garden
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
}

include(../../features/deploy.pri)

