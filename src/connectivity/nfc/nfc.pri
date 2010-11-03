
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
    nfc/qnearfieldmanagerimpl_p.h \
    nfc/qtlv_p.h \
    nfc/checksum_p.h

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
    nfc/qtlv.cpp \
    nfc/qllcpserver.cpp

simulator:PRIVATE_HEADERS += \
    nfc/qnearfieldmanager_simulator_p.h \
    nfc/qllcpsocket_simulator_p.h \
    nfc/qllcpserver_simulator_p.h

simulator:SOURCES += \
    nfc/qnearfieldmanager_simulator.cpp \
    nfc/qllcpsocket_simulator_p.cpp \
    nfc/qllcpserver_simulator_p.cpp

symbian:PRIVATE_HEADERS += \
    nfc/qllcpsocket_symbian_p.h \
    nfc/qllcpserver_symbian_p.h

symbian:SOURCES += \
    nfc/qllcpsocket_symbian_p.cpp \
    nfc/qllcpserver_symbian_p.cpp

!simulator:!symbian:PRIVATE_HEADERS += \
    nfc/qllcpsocket_p.h \
    nfc/qllcpserver_p.h

!simulator:!symbian:SOURCES += \
    nfc/qllcpsocket_p.cpp \
    nfc/qllcpserver_p.cpp

INCLUDEPATH += $$PWD
