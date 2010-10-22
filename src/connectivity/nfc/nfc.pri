
HEADERS += \
    nfc/qnearfieldmanager.h \
    nfc/qnearfieldtarget.h \
    nfc/qndefrecord.h \
    nfc/qndefnfctextrecord.h \
    nfc/qndefmessage.h \
    nfc/qllcpsocket.h \
    nfc/qndeffilter.h \
    nfc/qndefnfcurirecord.h \
    nfc/qnearfieldtagtype1.h \
    nfc/qnearfieldtagtype2.h \
    nfc/qllcpserver.h

PRIVATE_HEADERS += \
    nfc/qndefrecord_p.h \
    nfc/qnearfieldtarget_p.h \
    nfc/qnearfieldmanager_p.h \
    nfc/qnearfieldmanagerimpl_p.h

SOURCES += \
    nfc/qnearfieldmanager.cpp \
    nfc/qnearfieldtarget.cpp \
    nfc/qndefrecord.cpp \
    nfc/qndefnfctextrecord.cpp \
    nfc/qndefmessage.cpp \
    nfc/qllcpsocket.cpp \
    nfc/qndeffilter.cpp \
    nfc/qndefnfcurirecord.cpp \
    nfc/qnearfieldtagtype1.cpp \
    nfc/qnearfieldtagtype2.cpp \
    nfc/qllcpserver.cpp

simulator:HEADERS += \
    nfc/qnearfieldmanager_simulator_p.h

simulator:SOURCES += \
    nfc/qnearfieldmanager_simulator.cpp

INCLUDEPATH += $$PWD
    
symbian { 
    HEADERS += \
        nfc/qnearfieldmanager_symbian_p.h \
        nfc/qnearfieldtagtype1_symbian_p.h \
        nfc/symbian/nearfieldmanager_symbian.h \
        nfc/symbian/nearfieldtagtype1_symbian.h \
        nfc/symbian/nearfieldtargetfactory_symbian.h \
        nfc/symbian/llcpsocket_symbian.h

    SOURCES += \
        nfc/qnearfieldmanager_symbian.cpp \
        nfc/qnearfieldtagtype1_symbian.cpp \
        nfc/symbian/nearfieldmanager_symbian.cpp \	
        nfc/symbian/nearfieldtagtype1_symbian.cpp \
        nfc/symbian/nearfieldtargetfactory_symbian.cpp \
        nfc/symbian/llcpsocket_symbian.cpp

    INCLUDEPATH += $${EPOCROOT}epoc32/include/mw

    TARGET.CAPABILITY = ALL -TCB

    LIBS += -lnfc -lndef -lndefaccess -lnfcdiscoveryservice -lllcp -lnfctagextension
    }
