# Qt connectivity library
TEMPLATE = lib
TARGET   = QtConnectivity

include(../../common.pri)

DEFINES += QT_BUILD_CONNECTIVITY_LIB QT_MAKEDLL

QT = core

include(nfc/nfc.pri)
!mac:!win32:include(bluetooth/bluetooth.pri)

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian: {
    load(data_caging_paths)
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3=0x2002BFD1
    TARGET.CAPABILITY = All -TCB
}

simulator {
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}

include(../../features/deploy.pri)

