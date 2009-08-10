include(../../../common.pri)

TEMPLATE = lib
TARGET = support

HEADERS += \
    support.h


INCLUDEPATH += ../../../messaging

symbian|win32 {
symbian {
SOURCES += \
    support_symbian.cpp
}
win32 {

debug{
OUTPREFIX=Debug
} else {
OUTPREFIX=Release
}

DEFINES += QT_BUILD_MESSAGING_LIB
!static:DEFINES += QT_MAKEDLL
SOURCES += \
    support_win.cpp

LIBS += -L$$(OUT_PWD)../../../messaging/$$OUTPREFIX -lQtMessaging

}
} else {
# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

SOURCES += \
    support_qmf.cpp
}

