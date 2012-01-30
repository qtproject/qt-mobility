include(../../features/utils.pri)

# Qt connectivity library
TEMPLATE = lib
TARGET   = QtConnectivity

include(../../common.pri)
DEFINES += QT_BUILD_CONNECTIVITY_LIB QT_MAKEDLL

QT = core

include(connectivity.pri)
include(nfc/nfc.pri)
include(bluetooth/bluetooth.pri)

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian {
    load(data_caging_paths)
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = $$mobilityUID(0x2002BFD1)
    TARGET.CAPABILITY = All -TCB
}
symbian: {
    QtConnectivityDeployment.sources = QtConnectivity$${QT_LIBINFIX}.dll
    QtConnectivityDeployment.path = /sys/bin
    DEPLOYMENT += QtConnectivityDeployment
}

simulator {
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}
CONFIG += middleware
include(../../features/deploy.pri)

