TARGET = tst_qvaluespaceitem
INCLUDEPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input
SOURCES += tst_qvaluespaceitem.cpp

LIBS += -lQtContextFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*: {
    externApp.sources = ../process/vsextern.exe
    externApp.path = vsextern
    DEPLOYMENT += externApp
}
