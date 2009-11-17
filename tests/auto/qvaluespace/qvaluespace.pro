TARGET = tst_qvaluespace
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespace.cpp

qtAddLibrary(QtPublishSubscribe)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

maemo {
    DEFINES += Q_WS_MAEMO_6
}
