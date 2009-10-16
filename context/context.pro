TEMPLATE = lib
TARGET = QtPublishSubscribe
QT = core network

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL


PUBLIC_HEADERS += qcontextglobal.h \
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
    deploy.path = /
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders

    MMP_RULES += "EXPORTUNFROZEN"
}

unix:!symbian {
    maemo {
        SOURCES += contextkitlayer.cpp
        CONFIG += link_pkgconfig
        PKGCONFIG += contextsubscriber-1.0 QtDBus
    } else {
        HEADERS += qsystemreadwritelock_p.h \
           	   qmallocpool_p.h \
		   qpacketprotocol_p.h
        SOURCES += sharedmemorylayer.cpp \
           	   qmallocpool.cpp \
                   qsystemreadwritelock.cpp \
		   qpacketprotocol.cpp
    }
}

win32 {
    HEADERS += qsystemreadwritelock_p.h
    SOURCES += sharedmemorylayer.cpp \
               qsystemreadwritelock.cpp \
               registrylayer_win.cpp

    !wince*:LIBS += -ladvapi32
    wince*:LIBS += -ltoolhelp
}

headers.files = $$PUBLIC_HEADERS
headers.path = $$OUTPUT_DIR/include
INSTALLS+=headers
