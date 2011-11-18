CONFIG += mobility

PUBLIC_HEADERS += \
    nfc/qnearfieldmanager.h \
    nfc/qnearfieldtarget.h \
    nfc/qndefrecord.h \
    nfc/qndefnfctextrecord.h \
    nfc/qndefmessage.h \
    nfc/qndeffilter.h \
    nfc/qndefnfcurirecord.h \
    nfc/qnearfieldtagtype1.h \
    nfc/qnearfieldtagtype2.h \
    nfc/qllcpsocket.h \
    nfc/qnearfieldtagtype3.h \
    nfc/qnearfieldtagtype4.h \
    nfc/qllcpserver.h \
    nfc/qdeclarativendefrecord.h

PRIVATE_HEADERS += \
    nfc/qndefrecord_p.h \
    nfc/qnearfieldtarget_p.h \
    nfc/qnearfieldmanager_p.h \
    nfc/qtlv_p.h \
    nfc/checksum_p.h

SOURCES += \
    nfc/qnearfieldmanager.cpp \
    nfc/qnearfieldtarget.cpp \
    nfc/qndefrecord.cpp \
    nfc/qndefnfctextrecord.cpp \
    nfc/qndefmessage.cpp \
    nfc/qndeffilter.cpp \
    nfc/qndefnfcurirecord.cpp \
    nfc/qnearfieldtagtype1.cpp \
    nfc/qnearfieldtagtype2.cpp \
    nfc/qnearfieldtagtype3.cpp \
    nfc/qllcpsocket.cpp \
    nfc/qnearfieldtagtype4.cpp \
    nfc/qtlv.cpp \
    nfc/qllcpserver.cpp \
    nfc/qdeclarativendefrecord.cpp

maemo6|meego {
    NFC_BACKEND_AVAILABLE = yes

    QT *= dbus

    DBUS_INTERFACES += \
        nfc/maemo6/com.nokia.nfc.Manager.xml

    DBUS_ADAPTORS += \
        nfc/maemo6/com.nokia.nfc.AccessRequestor.xml \
        nfc/maemo6/com.nokia.nfc.NDEFHandler.xml

    # work around bug in Qt
    dbus_interface_source.depends = ${QMAKE_FILE_OUT_BASE}.h
    dbus_adaptor_source.depends = ${QMAKE_FILE_OUT_BASE}.h

    # Link against libdbus until Qt has support for passing file descriptors over DBus.
    CONFIG += link_pkgconfig
    DEFINES += DBUS_API_SUBJECT_TO_CHANGE
    PKGCONFIG += dbus-1

    PRIVATE_HEADERS += \
        nfc/qnearfieldmanager_maemo6_p.h \
        nfc/qnearfieldtarget_maemo6_p.h \
        nfc/qllcpsocket_maemo6_p.h \
        nfc/qllcpserver_maemo6_p.h \
        nfc/maemo6/adapter_interface_p.h \
        nfc/maemo6/target_interface_p.h \
        nfc/maemo6/tag_interface_p.h \
        nfc/maemo6/device_interface_p.h \
        nfc/maemo6/socketrequestor_p.h

    SOURCES += \
        nfc/qnearfieldmanager_maemo6.cpp \
        nfc/qnearfieldtarget_maemo6.cpp \
        nfc/qllcpsocket_maemo6_p.cpp \
        nfc/qllcpserver_maemo6_p.cpp \
        nfc/maemo6/adapter_interface.cpp \
        nfc/maemo6/target_interface.cpp \
        nfc/maemo6/tag_interface.cpp \
        nfc/maemo6/device_interface.cpp \
        nfc/maemo6/socketrequestor.cpp

    OTHER_FILES += \
        $$DBUS_INTERFACES \
        $$DBUS_ADAPTORS \
        nfc/maemo6/com.nokia.nfc.Adapter.xml \
        nfc/maemo6/com.nokia.nfc.Target.xml \
        nfc/maemo6/com.nokia.nfc.Tag.xml \
        nfc/maemo6/com.nokia.nfc.Device.xml \
        nfc/maemo6/com.nokia.nfc.LLCPRequestor.xml

    # Add OUT_PWD to INCLUDEPATH so that creator picks up headers for generated files
    # This is not needed for the build otherwise.
    INCLUDEPATH += $$OUT_PWD
}

simulator {
    NFC_BACKEND_AVAILABLE = yes

    QT *= gui

    PRIVATE_HEADERS += \
        nfc/qnearfieldmanagervirtualbase_p.h \
        nfc/qnearfieldmanager_simulator_p.h \
        nfc/qllcpsocket_simulator_p.h \
        nfc/qllcpserver_simulator_p.h

    SOURCES += \
        nfc/qnearfieldmanagervirtualbase.cpp \
        nfc/qnearfieldmanager_simulator.cpp \
        nfc/qllcpsocket_simulator_p.cpp \
        nfc/qllcpserver_simulator_p.cpp
}

contains(nfc_symbian_enabled, yes):symbian {
    NFC_BACKEND_AVAILABLE = yes

    MOBILITY = serviceframework
    INCLUDEPATH += $$SOURCE_DIR/src/serviceframework
    qtAddLibrary(QtServiceFramework)

    PRIVATE_HEADERS += \
        nfc/qnearfieldmanager_symbian_p.h \
        nfc/qnearfieldtagtype1_symbian_p.h \
        nfc/qnearfieldtagtype2_symbian_p.h \
        nfc/qllcpsocket_symbian_p.h \
        nfc/qllcpserver_symbian_p.h \
        nfc/qllcpstate_symbian_p.h \
        nfc/qnearfieldtagtype3_symbian_p.h \
        nfc/qnearfieldtagtype4_symbian_p.h \
        nfc/qnearfieldtagmifare_symbian_p.h \
        nfc/qnearfieldllcpdevice_symbian_p.h \
        nfc/symbian/nearfieldmanager_symbian.h \
        nfc/symbian/nearfieldtag_symbian.h \
        nfc/symbian/nearfieldndeftarget_symbian.h \
        nfc/symbian/nearfieldtargetfactory_symbian.h \
        nfc/symbian/nearfieldutility_symbian.h \
        nfc/symbian/llcpserver_symbian.h \
        nfc/symbian/llcpsockettype1_symbian.h \
        nfc/symbian/llcpsockettype2_symbian.h \
        nfc/symbian/nearfieldtagimpl_symbian.h \
        nfc/symbian/nearfieldtagasyncrequest_symbian.h \
        nfc/symbian/nearfieldtagndefoperationcallback_symbian.h \
        nfc/symbian/nearfieldtagoperationcallback_symbian.h \
        nfc/symbian/nearfieldtagndefrequest_symbian.h \
        nfc/symbian/nearfieldtagcommandrequest_symbian.h \
        nfc/symbian/nearfieldtagcommandsrequest_symbian.h \
        nfc/symbian/debug.h \
        nfc/symbian/nearfieldtagimplcommon_symbian.h

    SOURCES += \
        nfc/qnearfieldmanager_symbian.cpp \
        nfc/qnearfieldtagtype1_symbian.cpp \
        nfc/qnearfieldtagtype2_symbian.cpp \
        nfc/qllcpsocket_symbian_p.cpp \
        nfc/qllcpserver_symbian_p.cpp \
        nfc/qllcpstate_symbian_p.cpp \
        nfc/qnearfieldtagtype3_symbian.cpp \
        nfc/qnearfieldtagtype4_symbian.cpp \
        nfc/qnearfieldtagmifare_symbian.cpp \
        nfc/qnearfieldllcpdevice_symbian.cpp \
        nfc/symbian/nearfieldmanager_symbian.cpp \
        nfc/symbian/nearfieldtag_symbian.cpp \
        nfc/symbian/nearfieldndeftarget_symbian.cpp \
        nfc/symbian/nearfieldtargetfactory_symbian.cpp \
        nfc/symbian/nearfieldutility_symbian.cpp \
        nfc/symbian/llcpserver_symbian.cpp \
        nfc/symbian/llcpsockettype1_symbian.cpp \
        nfc/symbian/llcpsockettype2_symbian.cpp \
        nfc/symbian/nearfieldtagasyncrequest_symbian.cpp \
        nfc/symbian/nearfieldtagndefrequest_symbian.cpp \
        nfc/symbian/nearfieldtagcommandrequest_symbian.cpp \
        nfc/symbian/nearfieldtagcommandsrequest_symbian.cpp \
        nfc/symbian/nearfieldtagimplcommon_symbian.cpp

    INCLUDEPATH += $${EPOCROOT}epoc32/include/mw

    LIBS += -lnfc -lndef -lndefaccess -lnfcdiscoveryservice -lllcp -lnfctagextension -lcentralrepository
}

isEmpty(NFC_BACKEND_AVAILABLE) {
    # unsupported platform stub

    PRIVATE_HEADERS += \
        nfc/qllcpsocket_p.h \
        nfc/qllcpserver_p.h \
        nfc/qnearfieldmanagerimpl_p.h

    SOURCES += \
        nfc/qllcpsocket_p.cpp \
        nfc/qllcpserver_p.cpp \
        nfc/qnearfieldmanagerimpl_p.cpp
}

INCLUDEPATH += $$PWD
