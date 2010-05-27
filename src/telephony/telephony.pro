# #####################################################################
# Telephony Mobility API
# #####################################################################

TEMPLATE = lib

# Target gets fixed up in common.pri
TARGET = QtTelephony
DEFINES += QT_BUILD_TELEPHONY_LIB QT_MAKEDLL

include(../../common.pri)

# Input
PUBLIC_HEADERS += qtelephony.h \
                  qcallinfo.h
SOURCES +=        qtelephony.cpp \
                  qcallinfo.cpp

# Private Headers and sources
win32: {
    HEADERS += qtelephony_win_p.h 
    SOURCES += qtelephony_win.cpp 
}

symbian: {
    HEADERS += qtelephony_s60_p.h 
    SOURCES += qtelephony_s60.cpp 
} else {
    unix: {
        !maemo* {
            HEADERS += qtelephony_linux_p.h 
            SOURCES += qtelephony_linux.cpp
        }
        maemo* {
            HEADERS += qtelephony_maemo_p.h
            SOURCES += qtelephony_maemo.cpp
        }
    }
}

HEADERS += $$PUBLIC_HEADERS

include (../../features/deploy.pri)
