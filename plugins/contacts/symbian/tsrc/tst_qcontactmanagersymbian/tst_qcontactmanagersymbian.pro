TEMPLATE=app
TARGET=tst_qcontactmanagersymbian

QT += testlib
CONFIG += qtestlib
include(../tsrc.pri)

symbian: {
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$SYMBIAN_PATHS
    INCLUDEPATH += \epoc32\include\app
	INCLUDEPATH += \epoc32\include\platform\app
    
    SOURCES  += tst_qcontactmanagersymbian.cpp
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

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

    testdata = \
        "\"avatar_sharks_xs.jpg\" - \"!:\\data\\images\\avatar_sharks_xs.jpg\"" \
        "\"avatar_sharks_s.jpg\" - \"!:\\data\\images\\avatar_sharks_s.jpg\"" \
        "\"avatar_sharks_m.jpg\" - \"!:\\data\\images\\avatar_sharks_m.jpg\"" \
        "\"avatar_sharks_xxl.jpg\" - \"!:\\data\\images\\avatar_sharks_xxl.jpg\""
    testdeployment.pkg_postrules += testdata
    DEPLOYMENT += testdeployment 
}
