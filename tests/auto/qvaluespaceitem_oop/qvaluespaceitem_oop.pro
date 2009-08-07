TARGET = tst_qvaluespaceitem_oop
INCLUDEPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

VPATH += ../qvaluespaceitem
DEPENDPATH += ../qvaluespaceitem
INCLUDEPATH += ../qvaluespaceitem

# Input
SOURCES += tst_qvaluespaceitem_oop.cpp \
           tst_qvaluespaceitemshared.cpp
HEADERS += tst_qvaluespaceitemshared.h

LIBS += -lQtContextFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*: {
    externApp.sources = ../process/vsextern.exe
    externApp.path = vsextern
    DEPLOYMENT += externApp
}
