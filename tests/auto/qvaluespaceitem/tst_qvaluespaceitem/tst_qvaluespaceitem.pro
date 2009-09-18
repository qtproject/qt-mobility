TARGET = tst_qvaluespaceitem
INCLUDEPATH += ../../../../context

CONFIG+=testcase

QT = core

include(../../../../common.pri)

VPATH += ..
DEPENDPATH += ..
INCLUDEPATH += ..

# Input
SOURCES += tst_qvaluespaceitem.cpp \
           tst_qvaluespaceitemshared.cpp
HEADERS += tst_qvaluespaceitemshared.h

DEFINES += QT_START_VALUESPACE

qtAddLibrary(QtContextFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

