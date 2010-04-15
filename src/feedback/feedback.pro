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
PUBLIC_HEADERS += qfeedbackdevice.h qfeedbackeffect.h
PRIVATE_HEADERS += qfeedbackeffect_p.h

for(p, INCLUDEPATH):exists("$$p/ImmVibe.h"):CONFIG+=immersion

# Private Headers
SOURCES += qfeedbackdevice.cpp qfeedbackeffect.cpp
HEADERS += $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

contains(CONFIG, immersion) {
    SOURCES += qfeedbackeffect_immersion.cpp qfeedbackdevice_immersion.cpp
    LIBS += -lImmVibe
} else:symbian {
    SOURCES += qfeedbackdevice_symbian.cpp qfeedbackeffect_symbian.cpp
    LIBS += -ltouchfeedback -lhwrmvibraclient
} else {
    SOURCES += qfeedbackdevice_stub.cpp qfeedbackeffect_stub.cpp
}


symbian {
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
}
include(../../features/deploy.pri)

