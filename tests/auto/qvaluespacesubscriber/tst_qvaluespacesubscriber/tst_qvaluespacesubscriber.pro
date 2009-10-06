TARGET = tst_qvaluespacesubscriber
INCLUDEPATH += ../../../../context
DEPENDPATH += ../../../../context

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

LIBS += -lQtContextFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

win32 {
    !wince*:LIBS += -ladvapi32
}

wince* {
    DEPLOYLACKEY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/vsiTestLackey.exe
    DEPLOYLACKEY.path = .
    DEPLOYMENT += DEPLOYLACKEY
}
