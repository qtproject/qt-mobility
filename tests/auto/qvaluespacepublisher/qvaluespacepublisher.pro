TARGET = tst_qvaluespacepublisher
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespacepublisher.cpp

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
    crml.sources = qvaluespacepublisher.qcrml
    crml.path = /resource/qt/crml
    DEPLOYMENT += crml
    TARGET.UID3 = 0x2002ac87
    TARGET.CAPABILITY = ALL -TCB
}

win32 {
    !wince*:LIBS += -ladvapi32
}
