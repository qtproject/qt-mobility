TEMPLATE = lib
TARGET = QtPublishSubscribe
QT = core network

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL


PUBLIC_HEADERS += qcontextglobal.h \
           qvaluespace.h \
           qvaluespaceprovider.h \
           qvaluespacesubscriber.h

PRIVATE_HEADERS += qpacketprotocol_p.h \
           qmallocpool_p.h \
           qvaluespace_p.h \
           qvaluespacemanager_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qpacketprotocol.cpp \
           qmallocpool.cpp \
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
    HEADERS += qsystemreadwritelock_p.h
    SOURCES += sharedmemorylayer.cpp \
               qsystemreadwritelock.cpp
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
