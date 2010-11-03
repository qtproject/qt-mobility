# Qt connectivity library
TEMPLATE = lib
TARGET   = QtConnectivity

include(../../common.pri)

DEFINES += QT_BUILD_CONNECTIVITY_LIB QT_MAKEDLL

#disabled on symbian on Aaron's request
include(nfc/nfc.pri)
include(bluetooth/bluetooth.pri)

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian:TARGET.CAPABILITY = All -TCB

simulator {
    INCLUDEPATH += ../mobilitysimulator
    qtAddLibrary(QtMobilitySimulator)
}

include(../../features/deploy.pri)
