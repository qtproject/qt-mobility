TARGET = tst_qsystemreadwritelock_oop
INCLUDEPATH += ../../../../src/publishsubscribe

CONFIG+=testcase

QT = core network

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock_oop.cpp

CONFIG += mobility
MOBILITY = publishsubscribe

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince* {
    DEPLOYLACKEY.sources = $$OUTPUT_DIR/build/tests/bin/lackey.exe
    DEPLOYLACKEY.path = .
    DEPLOYMENT += DEPLOYLACKEY
}
