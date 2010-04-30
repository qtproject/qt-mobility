TARGET = tst_qvaluespacesubscriber
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../../common.pri)

VPATH += ..
DEPENDPATH += ..
INCLUDEPATH += ..

# Input
SOURCES += tst_qvaluespacesubscriber.cpp \
           tst_qvaluespacesubscribershared.cpp
HEADERS += tst_qvaluespacesubscribershared.h

DEFINES += QT_START_VALUESPACE

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
    crml.sources = qvaluespacesubscriber.qcrml
    crml.path = /resource/qt/crml
    DEPLOYMENT += crml
    TARGET.UID3 = 0x2002ac86
    TARGET.CAPABILITY = ALL -TCB
}

win32 {
    !wince*:LIBS += -ladvapi32
}

wince* {
    DEPLOYLACKEY.sources = $$OUTPUT_DIR/build/tests/bin/vsiTestLackey.exe
    DEPLOYLACKEY.path = .
    DEPLOYMENT += DEPLOYLACKEY
}
