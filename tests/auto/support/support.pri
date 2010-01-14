
HEADERS += \
    $$PWD/support.h

symbian|win32 {
    symbian {
        SOURCES += $$PWD/support_symbian.cpp
    }
    win32 {
        SOURCES += $$PWD/support_win.cpp

        wince*{
            LIBS += cemapi.lib
        }
        else {
            LIBS += mapi32.lib Advapi32.lib
        }

    }
} else {
    # QMF headers must be located at $QMF_INCLUDEDIR
    INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

    # QMF libraries must be located at $QMF_LIBDIR
    LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

    SOURCES += $$PWD/support_qmf.cpp
}

