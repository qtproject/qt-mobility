TEMPLATE = lib
TARGET = support

include(../../../common.pri)

INCLUDEPATH += ../../../messaging

LIBS += -lQtMessaging

DEFINES += QT_BUILD_MESSAGING_SUPPORT_LIB

HEADERS += \
    support.h

symbian|win32 {
    symbian {
        SOURCES += \
            support_symbian.cpp
    }
    win32 {
        !static:DEFINES += QT_MAKEDLL
        SOURCES += \
            support_win.cpp

        LIBS += mapi32.lib
    }
} else {
    # QMF headers must be located at $QMF_INCLUDEDIR
    INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

    # QMF libraries must be located at $QMF_LIBDIR
    LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

    SOURCES += \
        support_qmf.cpp
}

