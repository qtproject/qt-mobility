TEMPLATE = lib
TARGET = QtContextFramework
QT = core network gui

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL

HEADERS += qcontextglobal.h qpacketprotocol.h qvaluespace.h
SOURCES += qpacketprotocol.cpp qvaluespace.cpp

unix {
    HEADERS += qmallocpool.h \
               qsystemlock.h

    SOURCES += qmallocpool.cpp \
               qsystemlock.cpp \
               applayer.cpp
}

win32 {
    SOURCES += registrylayer_win.cpp

    LIBS += -ladvapi32
}
