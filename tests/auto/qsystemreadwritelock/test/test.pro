TARGET = tst_qsystemreadwritelock
INCLUDEPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

# Input 
SOURCES += ../tst_qsystemreadwritelock.cpp

qtAddLibrary(QtContextFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*: {
	externApp.sources = ../process/vsextern.exe
	externApp.path = vsextern
	DEPLOYMENT += externApp

}

