TEMPLATE = lib
TARGET = support

include(../../../common.pri)

INCLUDEPATH += ../../../messaging

LIBS += -lQtMessaging

HEADERS += \
    support.h

symbian|win32 {
    symbian {
        SOURCES += \
            support_symbian.cpp
    }
    win32 {
        DEFINES += QT_BUILD_MESSAGING_LIB
        !static:DEFINES += QT_MAKEDLL
        SOURCES += \
            support_win.cpp
    }
} else {
    # QMF headers must be located at $QMF_INCLUDEDIR
    INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

    # QMF libraries must be located at $QMF_LIBDIR
    LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

    SOURCES += \
        support_qmf.cpp
}

