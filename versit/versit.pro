# #####################################################################
# Versit API
# #####################################################################
TEMPLATE = lib
TARGET = QtVersit
DEFINES += BUILD_QTVERSIT \
    QT_ASCII_CAST_WARNINGS
include(../common.pri)

# Input
PUBLIC_HEADERS += qtversitglobal.h \
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
    qvcard21writer.h \
    qvcard30writer.h \
    qversitcontactexporter_p.h \
    qversitcontactimporter_p.h

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

qtAddLibrary(QtContacts)

symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
    
    # This is for new exporting system coming in garden
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
}
