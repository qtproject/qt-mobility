TEMPLATE=app
TARGET=tst_qcontactmanagersymbian

QT += testlib
CONFIG += qtestlib
include(../tsrc.pri)

symbian: {
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$SYMBIAN_PATHS
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    SOURCES  += tst_qcontactmanagersymbian.cpp
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lqtcontacts \
    		-lcntmodel

    BLD_INF_RULES.prj_exports += \
        "avatar_sharks_xs.jpg $${EPOCROOT}epoc32\winscw\c\data\images\avatar_sharks_xs.jpg"
    BLD_INF_RULES.prj_exports += \
        "avatar_sharks_s.jpg $${EPOCROOT}epoc32\winscw\c\data\images\avatar_sharks_s.jpg"
    BLD_INF_RULES.prj_exports += \
        "avatar_sharks_m.jpg $${EPOCROOT}epoc32\winscw\c\data\images\avatar_sharks_m.jpg"
    BLD_INF_RULES.prj_exports += \
        "avatar_sharks_xxl.jpg $${EPOCROOT}epoc32\winscw\c\data\images\avatar_sharks_xxl.jpg"
}
