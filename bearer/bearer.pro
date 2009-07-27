# Qt bearer management library
TEMPLATE = lib
TARGET = bearer

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h \
           qbearerglobal.h

SOURCES += qnetworksession.cpp \
           qnetworkconfigmanager.cpp \
           qnetworkconfiguration.cpp

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

    !mac:unix:SOURCES += qnetworkconfigmanager_unix.cpp \
                    qnetworkconfiguration_unix.cpp \
                    qnetworksession_unix.cpp

    !mac:unix:contains(QT_CONFIG,dbus): {
        QT += dbus
        HEADERS += qnmdbushelper_p.h qnetworkmanagerservice_p.h
        SOURCES += qnmdbushelper.cpp qnetworkmanagerservice_p.cpp
    }

    win32: {
        HEADERS += qnetworksessionengine_win_p.h \
                   qnlaengine_win_p.h

        !wince*:HEADERS += qnativewifiengine_win_p.h \
                           qioctlwifiengine_win_p.h

        SOURCES += qnetworkconfigmanager_win.cpp \
                   qnetworksession_win.cpp \
                   qnetworksessionengine_win.cpp \
                   qnlaengine_win.cpp

        !wince*:SOURCES += qnativewifiengine_win.cpp \
                           qioctlwifiengine_win.cpp

        !wince*:LIBS += -lWs2_32
        wince*:LIBS += -lWs2
    }
}

QT += network

maemo {
	QT += dbus
	CONFIG += link_pkgconfig

	target.path = $$[QT_INSTALL_PREFIX]/lib
	headers.files = $$HEADERS 
	headers.path = $$[QT_INSTALL_PREFIX]/include

	documentation.path = $$[QT_INSTALL_PREFIX]/share/doc/libbearer
 	documentation.files = doc/html

	INSTALLS += target headers documentation

	SOURCES += qnetworkconfigmanager_maemo.cpp \
		   qnetworksession_maemo.cpp

	SOURCES -= qnetworkconfigmanager_unix.cpp
	SOURCES -= qnetworkconfiguration_unix.cpp
	SOURCES -= qnetworksession_unix.cpp
	SOURCES -= qnmdbushelper.cpp
	SOURCES -= qnetworkmanagerservice_p.cpp
	OBJECTS -= qnetworkconfigmanager_unix.o
	OBJECTS -= qnetworkconfiguration_unix.o
	OBJECTS -= qnetworksession_unix.o
	OBJECTS -= qnmdbushelper.o
	OBJECTS -= qnetworkmanagerservice_p.o
	HEADERS -= qnmdbushelper_p.h
	HEADERS -= qnetworkmanagerservice_p.h

	PKGCONFIG += glib-2.0 dbus-glib-1 osso-ic conninet
	DEFINES += MAEMO

	# TODO: how to remove debian build dirs
	#QMAKE_DISTCLEAN += ../debian/libbearer ../debian/libbearer-dbg ../debian/libbearer-dev ../debian/libbearer-doc ../debian/libbearer-examples ../debian/libbearer-test ../debian/tmp ../debian/files ../debian/*.substvars ../debian/*.debhelper
}

include (../common.pri)

