TARGET = tst_qvaluespacesubscriber_oop
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

CONFIG+=testcase

QT = core

include(../../../../common.pri)

VPATH += ..
DEPENDPATH += ..
INCLUDEPATH += ..

# Input
SOURCES += tst_qvaluespacesubscriber_oop.cpp \
           tst_qvaluespacesubscribershared.cpp
HEADERS += tst_qvaluespacesubscribershared.h

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
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
