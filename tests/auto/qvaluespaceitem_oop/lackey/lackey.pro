SOURCES += main.cpp
TARGET = vsiTestLackey_oop
INCLUDEPATH += ../../../../context
QT = core

CONFIG+= testcase

include(../../../../common.pri)

LIBS += -lQtContextFramework
