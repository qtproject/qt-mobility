# Qt connectivity library
TEMPLATE = lib
TARGET   = QtConnectivity

include(../../common.pri)

DEFINES += QT_BUILD_CONNECTIVITY_LIB QT_MAKEDLL

QT = core

include(nfc/nfc.pri)
!win32:include(bluetooth/bluetooth.pri)

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian:TARGET.CAPABILITY = All -TCB

simulator {
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}

include(../../features/deploy.pri)
