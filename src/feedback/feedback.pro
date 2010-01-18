# #####################################################################
# Vibra Mobility API
# #####################################################################
TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtVibra
DEFINES += QT_BUILD_VIBRA_LIB QT_MAKEDLL\
    QT_ASCII_CAST_WARNINGS

include(../../common.pri)

# Input
PUBLIC_HEADERS += \

# Private Headers
PRIVATE_HEADERS += \

SOURCES += \

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    ## Need a new UID
    ## TARGET.UID3 = 0x2002AC7A

    ### Vibra
    # Main library
    VIBRA_DEPLOYMENT.sources = QtVibra.dll
    VIBRA_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += VIBRA_DEPLOYMENT

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include

    #export headers into EPOCROOT
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
}

include(../../features/deploy.pri)

