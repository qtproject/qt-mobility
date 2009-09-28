TEMPLATE = lib
TARGET = QtContextFramework
QT = core network gui

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL


PUBLIC_HEADERS += qcontextglobal.h \
           qvaluespace.h \
           qvaluespaceobject.h \
           qvaluespaceitem.h

PRIVATE_HEADERS += qpacketprotocol_p.h \
           qmallocpool_p.h \
           qvaluespace_p.h \
           qvaluespacemanager_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qpacketprotocol.cpp \
           qmallocpool.cpp \
           qvaluespace.cpp \
           qvaluespacemanager.cpp \
           qvaluespaceobject.cpp \
           qvaluespaceitem.cpp

symbian {
    deploy.path = /
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
}

unix {
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
