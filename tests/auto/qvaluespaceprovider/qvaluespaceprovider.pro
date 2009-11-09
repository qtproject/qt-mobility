TARGET = tst_qvaluespaceprovider
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qvaluespaceprovider.cpp

qtAddLibrary(QtPublishSubscribe)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

win32 {
    !wince*:LIBS += -ladvapi32
}
