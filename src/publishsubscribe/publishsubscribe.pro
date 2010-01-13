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
    DEPENDPATH += xqsettingsmanager_symbian
    INCLUDEPATH += xqsettingsmanager_symbian
    DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
    DEFINES += XQSETTINGSMANAGER_NO_TRANSACTIONS
    DEFINES += XQSETTINGSMANAGER_NO_CENREPKEY_CREATION_DELETION
    include(xqsettingsmanager_symbian/settingsmanager.pri)

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include

    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }

    DEPENDPATH += psmapperserver_symbian
    INCLUDEPATH += psmapperserver_symbian

    HEADERS += pathmapper_symbian_p.h \
        pathmapper_proxy_symbian_p.h
    LIBS += -lefsrv

    DEFINES += QT_BUILD_INTERNAL
    HEADERS += settingslayer_symbian_p.h
    SOURCES += settingslayer_symbian.cpp

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC78

    QtPublishSubscribeDeployment.sources = QtPublishSubscribe.dll PSPathMapperServer.exe
    QtPublishSubscribeDeployment.path = /sys/bin
    DEPLOYMENT += QtPublishSubscribeDeployment
}

unix:!symbian {
    maemo6 {
        SOURCES += contextkitlayer.cpp
        CONFIG += link_pkgconfig
        PKGCONFIG += contextsubscriber-1.0 QtDBus
    } else {
        HEADERS += gconflayer_linux_p.h
        SOURCES += gconflayer_linux.cpp
        CONFIG += link_pkgconfig
        PKGCONFIG += glib-2.0 gconf-2.0

        #As a workaround build GConfItem wrapper class with the project
        HEADERS += gconfitem.h
        SOURCES += gconfitem.cpp

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
