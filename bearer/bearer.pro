# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h \
           qbearerglobal.h

SOURCES += qnetworksession.cpp \
           qnetworkconfigmanager.cpp \
           qnetworkconfiguration.cpp

unix:!symbian:!mac:contains(BACKEND, NetworkManager) {
    unix:contains(QT_CONFIG,dbus) {
        DEFINES+=BACKEND_NM
    } else {
        message("NetworkManager backend requires Qt DBus support");
    }
}


symbian: {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE=1
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
    }
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += qnetworkconfigmanager_s60_p.h \
               qnetworkconfiguration_s60_p.h \
               qnetworksession_s60_p.h
    SOURCES += qnetworkconfigmanager_s60_p.cpp \
               qnetworkconfiguration_s60_p.cpp \
               qnetworksession_s60_p.cpp
               
    LIBS += -lcommdb \
            -lapengine \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -lecom \
            -lefsrv \
            -lnetmeta

    TARGET.CAPABILITY = All -TCB
} else {
    HEADERS += qnetworkconfigmanager_p.h \
               qnetworkconfiguration_p.h \
               qnetworksession_p.h

    win32:DEFINES += BEARER_ENGINE
    unix:DEFINES += BEARER_ENGINE

    contains(DEFINES, BEARER_ENGINE) {
        HEADERS += qnetworksessionengine_p.h \
                   qgenericengine_p.h

        SOURCES += qnetworkconfigmanager_p.cpp \
                   qnetworksession_p.cpp \
                   qnetworksessionengine.cpp \
                   qgenericengine.cpp
    }

    !mac:unix:contains(QT_CONFIG,dbus):contains(BACKEND, NetworkManager) {
        QT += dbus
        HEADERS += qnmdbushelper_p.h \
                   qnetworkmanagerservice_p.h \
                   qnmwifiengine_unix_p.h

        SOURCES += qnmdbushelper.cpp \
                   qnetworkmanagerservice_p.cpp \
                   qnmwifiengine_unix.cpp
    }

    win32: {
        HEADERS += qnlaengine_win_p.h

        !wince*:HEADERS += qnativewifiengine_win_p.h

        SOURCES += qnlaengine_win.cpp

        !wince*:SOURCES += qnativewifiengine_win.cpp

        !wince*:LIBS += -lWs2_32
        wince*:LIBS += -lWs2
    }
}

QT += network

include (../common.pri)

