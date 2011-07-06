TEMPLATE = app

TARGET = tst_qsysteminfo_s60_p

QT += testlib

CONFIG += qtestlib
CONFIG += console

include(../../../../../common.pri)

symbian:
{
    INCLUDEPATH += ../../ ../../../
    VPATH += ../../
    HEADERS  += qsysteminfo_s60_p.h
    SOURCES  += tst_qsysteminfo_s60_p.cpp

    # From systeminfo.pro
    contains(S60_VERSION, 3.1) {
        DEFINES += SYMBIAN_3_1
    }

    contains(S60_VERSION, 5.2) {
        DEFINES += SYMBIAN_3_PLATFORM
    }

    contains(LockandFlipPSkeys_enabled, yes) {
        message("LockandFlipPSKeys available")
        DEFINES += LOCKANDFLIP_SUPPORTED
        SOURCES += lockandflipstatus_s60.cpp
        HEADERS += lockandflipstatus_s60.h
        }

    contains(FmTxClient_enabled, yes) {
        message("FmTxClient available")
        DEFINES += FMTXCLIENT_SUPPORTED
        LIBS += -lhwrmfmtxclient
    }

    contains(DiskNotifyClient_enabled, yes) {
        message("DiskNotiferClient available")
        DEFINES += DISKNOTIFY_SUPPORTED
        LIBS += -ldisknotifyhandler
        SOURCES += storagedisknotifier_s60.cpp
        HEADERS += storagedisknotifier_s60.h
    }

    contains(hb_symbian_enabled, yes) {
        ## for symbian ^4
        CONFIG += qt hb
        DEFINES += HB_SUPPORTED
        message("s60_HbKeymap enabled")
        LIBS += -lhbcore
    } else {
        LIBS += -lptiengine
    }

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    DEPENDPATH += symbian

    SOURCES += \
        #qsysteminfo_s60.cpp \
        telephonyinfo_s60.cpp \
        chargingstatus_s60.cpp \
        wlaninfo_s60.cpp \
        storagestatus_s60.cpp \
        pubandsubkey_s60.cpp \
        batterystatus_s60.cpp \
        networkinfo_s60.cpp

    HEADERS += \
        #qsysteminfo_s60_p.h \
        telephonyinfo_s60.h \
        chargingstatus_s60.h \
        wlaninfo_s60.h \
        storagestatus_s60.h \
        pubandsubkey_s60.h \
        batterystatus_s60.h \
        networkinfo_s60.h

    LIBS += \
        -lprofileengine \
        -letel3rdparty \
        -lsysutil \
        -lcentralrepository \
        -lcenrepnotifhandler \
        -lefsrv \
        -lfeatdiscovery \
        -lhwrmvibraclient \
        -lavkon \    #Used by AknLayoutUtils::PenEnabled(). Try to remove this dependency.
        -lcone \
        -lws32 \
        -lcentralrepository \
        -lbluetooth \
        -lgdi \
        -lecom \
        -lplatformenv \
        -lhwrmlightclient \
        -letel

    contains(S60_VERSION, 5.1) | contains(S60_VERSION, 5.2) {
        LIBS += -lhwrmpowerclient -lusbman
    }

    contains(symbiancntsim_enabled, yes) {
        LIBS += -letelmm
        DEFINES += ETELMM_SUPPORTED
        message("ETELMM enabled")
    }

    contains(etelpacketservice_symbian_enabled, yes) {
        message("etel packet service enabled")
        LIBS += -letelpckt
        DEFINES += ETELPACKETSERVICE_SUPPORTED
    }

    contains(thermalstatus_symbian_enabled, yes) {
        DEFINES += THERMALSTATUS_SUPPORTED
        SOURCES += thermalstatus_s60.cpp
        HEADERS += thermalstatus_s60.h
        message("Thermalstatus enabled")
    }

    TARGET.CAPABILITY = ALL -TCB
}