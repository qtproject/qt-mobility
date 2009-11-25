TEMPLATE = lib
TARGET = QtPublishSubscribe
QT = core network

include(../../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL


PUBLIC_HEADERS += \
           qvaluespace.h \
           qvaluespacepublisher.h \
           qvaluespacesubscriber.h

PRIVATE_HEADERS += \
           qvaluespace_p.h \
           qvaluespacemanager_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
           qvaluespace.cpp \
           qvaluespacemanager.cpp \
           qvaluespacepublisher.cpp \
           qvaluespacesubscriber.cpp

symbian {
    DEPENDPATH += symbian
    INCLUDEPATH += symbian
    DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
    include(symbian/settingsmanager.pri)

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include

    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }

    DEFINES += QT_BUILD_INTERNAL
    HEADERS += settingslayer_symbian.h \
        pathmapper_symbian.h \
        qcrmlparser_p.h
    SOURCES += settingslayer_symbian.cpp \
        pathmapper_symbian.cpp \
        qcrmlparser.cpp
    MMP_RULES += "EXPORTUNFROZEN"
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC78

    QtPublishSubscribeDeployment.sources = QtPublishSubscribe.dll
    QtPublishSubscribeDeployment.path = /sys/bin

    DEPLOYMENT += QtPublishSubscribeDeployment
}

unix:!symbian {
    maemo {
        DEFINES += Q_WS_MAEMO_6
        SOURCES += contextkitlayer.cpp
        CONFIG += link_pkgconfig
        PKGCONFIG += contextsubscriber-1.0 QtDBus
    } else {
        HEADERS += qsystemreadwritelock_p.h \
           	   qmallocpool_p.h \
		   qpacketprotocol_p.h
        SOURCES += sharedmemorylayer.cpp \
           	   qmallocpool.cpp \
                   qsystemreadwritelock_unix.cpp \
		   qpacketprotocol.cpp
    }
}

win32 {
    HEADERS += qsystemreadwritelock_p.h
    SOURCES += \
               qsystemreadwritelock_win.cpp \
               registrylayer_win.cpp

    !wince*:LIBS += -ladvapi32
    wince*:LIBS += -ltoolhelp
}

include(../../features/deploy.pri)
