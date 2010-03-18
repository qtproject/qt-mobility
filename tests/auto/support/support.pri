
HEADERS += \
    $$PWD/support.h

symbian|wince*|maemo5|win32|mac {
    symbian {
        SOURCES += $$PWD/support_symbian.cpp
        INCLUDEPATH += $$(EPOCROOT)epoc32/include/app
        INCLUDEPATH += $$(EPOCROOT)epoc32/include/platform/app
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
    maemo5|mac {
         SOURCES += $$PWD/support_stub.cpp       
    }
} else {
    # QMF headers must be located at $QMF_INCLUDEDIR
    INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

    # QMF libraries must be located at $QMF_LIBDIR
    LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

    SOURCES += $$PWD/support_qmf.cpp
}

