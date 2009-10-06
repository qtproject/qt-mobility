TARGET = tst_qvaluespaceitem_oop
INCLUDEPATH += ../../../../context
DEPENDPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

VPATH += ..
DEPENDPATH += ..
INCLUDEPATH += ..

# Input
SOURCES += tst_qvaluespaceitem_oop.cpp \
           tst_qvaluespaceitemshared.cpp
HEADERS += tst_qvaluespaceitemshared.h

qtAddLibrary(QtContextFramework)

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
