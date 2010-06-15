# #####################################################################
# Telephony Mobility API
# #####################################################################

TEMPLATE = lib
TARGET = QtTelephony

include(../../common.pri)

# Target gets fixed up in common.pri
DEFINES += QT_BUILD_TELEPHONY_LIB QT_MAKEDLL

# Input
PUBLIC_HEADERS += qtelephonycalllist.h \
                  qtelephonycallinfo.h

SOURCES +=        qtelephonycalllist.cpp \
                  qtelephonycallinfo.cpp

# Private Headers and sources
win32: {
    HEADERS += qtelephonycalllist_win_p.h
    SOURCES += qtelephonycalllist_win.cpp
    HEADERS += qtelephonycallinfo_win_p.h
    SOURCES += qtelephonycallinfo_win.cpp
}

symbian: {
    HEADERS += qtelephonycalllist_s60_p.h
    SOURCES += qtelephonycalllist_s60.cpp
    HEADERS += qtelephonycallinfo_s60_p.h
    SOURCES += qtelephonycallinfo_s60.cpp
} else {
    unix: {
        !maemo* {
            HEADERS += qtelephonycalllist_linux_p.h
            SOURCES += qtelephonycalllist_linux.cpp
            HEADERS += qtelephonycallinfo_linux_p.h
            SOURCES += qtelephonycallinfo_linux.cpp
        }
        maemo* {
            HEADERS += qtelephonycalllist_maemo_p.h
            SOURCES += qtelephonycalllist_maemo.cpp
            HEADERS += qtelephonycallinfo_maemo_p.h
            SOURCES += qtelephonycallinfo_maemo.cpp
        }
    }
}

HEADERS += $$PUBLIC_HEADERS

include (../../features/deploy.pri)
