
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
    nfc/qnearfieldtagtype3.h \
    nfc/qnearfieldtagtype4.h \
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
    nfc/qnearfieldtagtype3.cpp \
    nfc/qnearfieldtagtype4.cpp \
    nfc/qtlv.cpp \
    nfc/qllcpserver.cpp

maemo6|meego {
    QT *= dbus

    DBUS_INTERFACES += \
        nfc/meego/com.nokia.nfc.Manager.xml \
        nfc/meego/com.nokia.nfc.Device.xml

    DBUS_ADAPTORS += \
        nfc/meego/com.nokia.nfc.AccessRequestor.xml \
        nfc/meego/com.nokia.nfc.LLCPRequestor.xml \
        nfc/meego/com.nokia.nfc.NDEFHandler.xml

    # work around bug in Qt
    dbus_interface_source.depends = ${QMAKE_FILE_OUT_BASE}.h
    dbus_adaptor_source.depends = ${QMAKE_FILE_OUT_BASE}.h

    PRIVATE_HEADERS += \
        nfc/qnearfieldmanager_meego_p.h \
        nfc/qnearfieldtarget_meego_p.h \
        nfc/qllcpsocket_meego_p.h \
        nfc/qllcpserver_meego_p.h \
        nfc/meego/adapter_interface_p.h \
        nfc/meego/target_interface_p.h \
        nfc/meego/tag_interface_p.h

    SOURCES += \
        nfc/qnearfieldmanager_meego.cpp \
        nfc/qnearfieldtarget_meego.cpp \
        nfc/qllcpsocket_meego_p.cpp \
        nfc/qllcpserver_meego_p.cpp \
        nfc/meego/adapter_interface.cpp \
        nfc/meego/target_interface.cpp \
        nfc/meego/tag_interface.cpp

    OTHER_FILES += \
        $$DBUS_INTERFACES \
        $$DBUS_ADAPTORS \
        nfc/meego/com.nokia.nfc.Adapter.xml \
        nfc/meego/com.nokia.nfc.Target.xml \
        nfc/meego/com.nokia.nfc.Tag.xml

    # Add OUT_PWD to INCLUDEPATH so that creator picks up headers for generated files
    # This is not needed for the build otherwise.
    INCLUDEPATH += $$OUT_PWD
}

simulator {
    QT *= gui

    PRIVATE_HEADERS += \
        nfc/qnearfieldmanager_simulator_p.h \
        nfc/qllcpsocket_simulator_p.h \
        nfc/qllcpserver_simulator_p.h

    SOURCES += \
        nfc/qnearfieldmanager_simulator.cpp \
        nfc/qllcpsocket_simulator_p.cpp \
        nfc/qllcpserver_simulator_p.cpp
}

symbian {
    PRIVATE_HEADERS += \
        nfc/qnearfieldmanager_symbian_p.h \
        nfc/qllcpsocket_symbian_p.h \
        nfc/qllcpserver_symbian_p.h

    SOURCES += \
        nfc/qllcpsocket_symbian_p.cpp \
        nfc/qllcpserver_symbian_p.cpp
}

!meego:!maemo6:!simulator:!symbian {
    # unsupported platform stub

    PRIVATE_HEADERS += \
        nfc/qllcpsocket_p.h \
        nfc/qllcpserver_p.h

    SOURCES += \
        nfc/qllcpsocket_p.cpp \
        nfc/qllcpserver_p.cpp
}

INCLUDEPATH += $$PWD
