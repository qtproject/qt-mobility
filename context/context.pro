TEMPLATE = lib
TARGET = QtContextFramework
QT = core network gui

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL

HEADERS += qcontextglobal.h \
           qpacketprotocol.h \
           qvaluespace.h \
           qvaluespacemanager_p.h \
           qvaluespaceobject.h

SOURCES += qpacketprotocol.cpp \
           qvaluespace.cpp \
           qvaluespacemanager.cpp \
           qvaluespaceobject.cpp

unix {
    HEADERS += qmallocpool.h \
               qsystemlock.h

    SOURCES += qmallocpool.cpp \
               qsystemlock.cpp \
               sharedmemorylayer.cpp
}

win32 {
    SOURCES += registrylayer_win.cpp

    LIBS += -ladvapi32
}
