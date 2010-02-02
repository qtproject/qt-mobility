TARGET = tst_serviceframework
CONFIG+=testcase

SOURCES  += tst_serviceframework.cpp
QT = core network
LIBS += -L../../../lib -lQtDBus -lQtXml -lQtGui -lQtSql
INCLUDEPATH += ../../../src/serviceframework

include(../../../common.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = All -TCB
}
