TARGET = tst_qsystemreadwritelock_oop
INCLUDEPATH += ../../../../context

CONFIG+=testcase

QT = core network

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock_oop.cpp

LIBS += -lQtPublishSubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince* {
    DEPLOYLACKEY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/lackey.exe
    DEPLOYLACKEY.path = .
    DEPLOYMENT += DEPLOYLACKEY
}
