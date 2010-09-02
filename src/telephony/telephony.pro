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
                  qtelephonycallinfo.h \
                  qtelephony.h

SOURCES +=        qtelephonycalllist.cpp \
                  qtelephonycallinfo.cpp \
                  qtelephony.cpp

# Private Headers and sources

symbian {
    contains(callinformation_symbian_enabled, yes) {
        # Implementation which is used with Symbian^3 and Symbian^4
        HEADERS += symbian/qtelephonycalllist_symbian_p.h \
                   symbian/qtelephonycallinfo_symbian_p.h
        SOURCES += symbian/qtelephonycalllist_symbian_p.cpp \
                   symbian/qtelephonycallinfo_symbian_p.cpp
               
        LIBS += -ltelephonyservice -lserviceprovidersettings
    } else {
        HEADERS += qtelephonycalllist_symbian_p.h \
                   qtelephonycallinfo_p.h
        SOURCES += qtelephonycalllist_symbian.cpp
    }
    
    # Export headers to middleware
    CONFIG += middleware

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002BFCF

    QtTelephony.sources = QtTelephony.dll
    QtTelephony.path = /sys/bin
    DEPLOYMENT += QtTelephony
}
 
linux-*:!maemo* {
    QT += dbus
    HEADERS += linux/qtelephonycalllist_linux_p.h \
               qtelephonycallinfo_p.h

    SOURCES += linux/qtelephonycalllist_linux.cpp
    SOURCES +=  linux/telepathy.cpp \
                linux/telepathylistener.cpp \
                linux/message.cpp \
                linux/dbusadaptor.cpp \
                linux/dbusinterface.cpp
    HEADERS +=  linux/telepathy_p.h \
                linux/telepathylistener_p.h \
                linux/message_p.h \
                linux/dbusadaptor_p.h \
                linux/dbusinterface_p.h
}
maemo* {
    QT += dbus
    include(maemo.pri)
}
mac|win32 {
    HEADERS += qtelephonycalllist_unsupported_p.h \
               qtelephonycallinfo_p.h
}

HEADERS += $$PUBLIC_HEADERS

include (../../features/deploy.pri)
