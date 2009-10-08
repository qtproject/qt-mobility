# #####################################################################
# Contacts Mobility API
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
    qversitcontactconverter.h \
    qversitcontactgenerator.h

# Private Headers
PRIVATE_HEADERS += qversitdocument_p.h \
    qversitproperty_p.h \
    qversitreader_p.h \
    qversitwriter_p.h \
    qversitcontactconverter_p.h \
    qversitcontactgenerator_p.h

SOURCES += qversitdocument.cpp \
    qversitproperty.cpp \
    qversitreader.cpp \
    qversitwriter.cpp \
    qversitcontactconverter.cpp \
    qversitcontactgenerator.cpp \

LIBS += -lQtContacts

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
