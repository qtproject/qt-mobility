# #####################################################################
# Feedback Mobility API
# #####################################################################
TEMPLATE = lib

QT = core

# Target gets fixed up in common.pri
TARGET = QtFeedback
DEFINES += QT_BUILD_FEEDBACK_LIB \
    QT_MAKEDLL \
    QT_ASCII_CAST_WARNINGS
include(../../common.pri)
INCLUDEPATH += .

# Input
PUBLIC_HEADERS += qfeedbackactuator.h qfeedbackeffect.h qfeedbackplugininterfaces.h
PRIVATE_HEADERS += qfeedbackeffect_p.h qfeedbackplugin_p.h

SOURCES += qfeedbackactuator.cpp qfeedbackeffect.cpp qfeedbackplugin.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    
    # UID
    TARGET.UID3 = 0x2002BFCE

    # Main library
    FEEDBACK_DEPLOYMENT.sources = QtFeedback.dll
    FEEDBACK_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += FEEDBACK_DEPLOYMENT
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    
    # export headers into EPOCROOT
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
}
include(../../features/deploy.pri)

