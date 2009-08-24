TEMPLATE = lib
TARGET = QtContextFramework
QT = core network gui

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL

HEADERS += qcontextglobal.h \
           qpacketprotocol.h \
           qmallocpool.h \
           qvaluespace.h \
           qvaluespace_p.h \
           qvaluespacemanager_p.h \
           qvaluespaceobject.h

SOURCES += qpacketprotocol.cpp \
           qmallocpool.cpp \
           qvaluespace.cpp \
           qvaluespacemanager.cpp \
           qvaluespaceobject.cpp

unix {
    HEADERS += qsystemreadwritelock.h

    SOURCES += sharedmemorylayer.cpp \
               qsystemreadwritelock.cpp
}

win32 {
    SOURCES += registrylayer_win.cpp

    LIBS += -ladvapi32
}
