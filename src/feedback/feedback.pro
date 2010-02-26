# #####################################################################
# Feedback Mobility API
# #####################################################################
TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtFeedback
DEFINES += QT_BUILD_FEEDBACK_LIB \
    QT_MAKEDLL \
    QT_ASCII_CAST_WARNINGS
include(../../common.pri)
INCLUDEPATH += .

# Input
PUBLIC_HEADERS += qfeedbackeffect.h qfeedbackeffect_p.h

# Private Headers
PRIVATE_HEADERS += 
SOURCES += qfeedbackeffect.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS
symbian { 
    SOURCES += qfeedbackeffect_symbian.cpp
    LIBS += -ltouchfeedback -lhwrmvibraclient
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    
    # # Need a new UID
    # # TARGET.UID3 = 0x2002AC7A
    # ## Feedback
    # Main library
    FEEDBACK_DEPLOYMENT.sources = QtFeedback.dll
    FEEDBACK_DEPLOYMENT.path = \sys\bin
    DEPLOYMENT += FEEDBACK_DEPLOYMENT
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    
    # export headers into EPOCROOT
    for(header, exportheaders.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
} else:SOURCES += qfeedbackeffect_stub.cpp
include(../../features/deploy.pri)
