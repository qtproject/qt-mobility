TARGET = tst_qservicemanager_ipc
INCLUDEPATH += ../../../../src/serviceframework
DEPENDPATH += ../../../../src/serviceframework

QT = core testlib
CONFIG += testcase
TEMPLATE = app

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += tst_qservicemanager_ipc.cpp
HEADERS += 

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
