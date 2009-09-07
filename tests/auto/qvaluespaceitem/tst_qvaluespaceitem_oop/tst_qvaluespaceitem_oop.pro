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

LIBS += -lQtContextFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

