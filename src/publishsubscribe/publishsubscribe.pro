TEMPLATE = lib
TARGET = QtPublishSubscribe
QT = core network xml

include(../../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL


PUBLIC_HEADERS += \
           qvaluespace.h \
           qvaluespaceprovider.h \
           qvaluespacesubscriber.h

PRIVATE_HEADERS += \
           qvaluespace_p.h \
           qvaluespacemanager_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
           qvaluespace.cpp \
           qvaluespacemanager.cpp \
           qvaluespaceprovider.cpp \
           qvaluespacesubscriber.cpp

symbian {
    HEADERS += qcrmlparser_p.h
    SOURCES += qcrmlparser.cpp
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders

    MMP_RULES += "EXPORTUNFROZEN"
    TARGET.CAPABILITY = ALL -TCB
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
