
PUBLIC_HEADERS += \
    bluetooth/qbluetoothaddress.h\
    bluetooth/qbluetoothuuid.h\
    bluetooth/qbluetoothdeviceinfo.h\
    bluetooth/qbluetoothserviceinfo.h\
    bluetooth/qbluetoothdevicediscoveryagent.h\
    bluetooth/qbluetoothservicediscoveryagent.h\
    bluetooth/qbluetoothsocket.h\
    bluetooth/qrfcommserver.h \
    bluetooth/ql2capserver.h \
    bluetooth/qbluetooth.h \
    bluetooth/qbluetoothlocaldevice.h \
    bluetooth/ql2capsocket.h \
    bluetooth/qrfcommsocket.h \
    bluetooth/qbluetoothtransfermanager.h \
    bluetooth/qbluetoothtransferrequest.h \
    bluetooth/qbluetoothtransferreply.h

PRIVATE_HEADERS += \
    bluetooth/qbluetoothaddress_p.h\
    bluetooth/qbluetoothdeviceinfo_p.h\
    bluetooth/qbluetoothserviceinfo_p.h\
    bluetooth/qbluetoothdevicediscoveryagent_p.h\
    bluetooth/qbluetoothservicediscoveryagent_p.h\
    bluetooth/qbluetoothsocket_p.h\
    bluetooth/qrfcommserver_p.h \
    bluetooth/ql2capserver_p.h \
    bluetooth/qbluetoothtransferreply_p.h \
    bluetooth/qbluetoothtransferrequest_p.h

SOURCES += \
    bluetooth/qbluetoothaddress.cpp\
    bluetooth/qbluetoothuuid.cpp\
    bluetooth/qbluetoothdeviceinfo.cpp\
    bluetooth/qbluetoothserviceinfo.cpp\
    bluetooth/qbluetoothdevicediscoveryagent.cpp\
    bluetooth/qbluetoothservicediscoveryagent.cpp\
    bluetooth/qbluetoothsocket.cpp\
    bluetooth/qrfcommserver.cpp \
    bluetooth/ql2capserver.cpp \
    bluetooth/qbluetoothlocaldevice.cpp \
    bluetooth/qbluetooth.cpp \
    bluetooth/ql2capsocket.cpp \
    bluetooth/qrfcommsocket.cpp \
    bluetooth/qbluetoothtransfermanager.cpp \
    bluetooth/qbluetoothtransferrequest.cpp \
    bluetooth/qbluetoothtransferreply.cpp

symbian {
    contains(S60_VERSION, 3.1) | contains(S60_VERSION, 3.2) {
        DEFINES += DO_NOT_BUILD_BLUETOOTH_SYMBIAN_BACKEND
        message("S60 3.1 or 3.2 sdk not supported by bluetooth")
        SOURCES += \
            bluetooth/qbluetoothdevicediscoveryagent_p.cpp \
            bluetooth/qbluetoothlocaldevice_p.cpp \
            bluetooth/qbluetoothserviceinfo_p.cpp \
            bluetooth/qbluetoothservicediscoveryagent_p.cpp \
            bluetooth/qbluetoothsocket_p.cpp \
            bluetooth/ql2capserver_p.cpp \
            bluetooth/qrfcommserver_p.cpp \
            bluetooth/qbluetoothtransfermanager_p.cpp
    }
}

symbian {
    !contains(DEFINES, DO_NOT_BUILD_BLUETOOTH_SYMBIAN_BACKEND) {
        DEFINES += QTM_SYMBIAN_BLUETOOTH
        INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
        include(symbian/symbian.pri)

        PRIVATE_HEADERS += \
            bluetooth/qbluetoothtransferreply_symbian_p.h \
            bluetooth/qbluetoothlocaldevice_p.h

        SOURCES += \
            bluetooth/qbluetoothserviceinfo_symbian.cpp\
            bluetooth/qbluetoothdevicediscoveryagent_symbian.cpp\
            bluetooth/qbluetoothservicediscoveryagent_symbian.cpp\
            bluetooth/qbluetoothsocket_symbian.cpp\
            bluetooth/qrfcommserver_symbian.cpp \
            bluetooth/qbluetoothlocaldevice_symbian.cpp \
            bluetooth/qbluetoothtransfermanager_symbian.cpp \
            bluetooth/qbluetoothtransferreply_symbian.cpp \
            bluetooth/ql2capserver_symbian.cpp

        contains(S60_VERSION, 5.0) {
            message("NOTICE - START")
            message("Bluetooth backend needs SDK plugin from Forum Nokia for 5.0 SDK")
            message("NOTICE - END")
            LIBS *= -lirobex
        } else {
            LIBS *= -lobex
        }
        LIBS *= -lesock \
                -lbluetooth \
                -lsdpagent \
                -lsdpdatabase \
                -lestlib \
                -lbtengsettings \
                -lbtmanclient \
                -lbtdevice
    }
} else:contains(bluez_enabled, yes):contains(QT_CONFIG, dbus) {
    QT *= dbus
    DEFINES += QTM_BLUEZ_BLUETOOTH

    include(bluez/bluez.pri)

    PRIVATE_HEADERS += \
        bluetooth/qbluetoothtransferreply_bluez_p.h \
        bluetooth/qbluetoothlocaldevice_p.h

    SOURCES += \
        bluetooth/qbluetoothserviceinfo_bluez.cpp \
        bluetooth/qbluetoothdevicediscoveryagent_bluez.cpp\
        bluetooth/qbluetoothservicediscoveryagent_bluez.cpp \
        bluetooth/qbluetoothsocket_bluez.cpp \
        bluetooth/qrfcommserver_bluez.cpp \
        bluetooth/qbluetoothlocaldevice_bluez.cpp \
        bluetooth/qbluetoothtransferreply_bluez.cpp \
        bluetooth/qbluetoothtransfermanager_bluez.cpp \
        bluetooth/ql2capserver_bluez.cpp
} else {
    message("Unsupported bluetooth platform, will not build a working QBluetooth library")
    message("Either no Qt dBus found, no bluez headers, or not symbian")
    SOURCES += \
        bluetooth/qbluetoothdevicediscoveryagent_p.cpp \
        bluetooth/qbluetoothlocaldevice_p.cpp \
        bluetooth/qbluetoothserviceinfo_p.cpp \
        bluetooth/qbluetoothservicediscoveryagent_p.cpp \
        bluetooth/qbluetoothsocket_p.cpp \
        bluetooth/ql2capserver_p.cpp \
        bluetooth/qrfcommserver_p.cpp \
        bluetooth/qbluetoothtransfermanager_p.cpp

}

INCLUDEPATH += $$PWD
INCLUDEPATH += ..

QT += network

OTHER_FILES +=
