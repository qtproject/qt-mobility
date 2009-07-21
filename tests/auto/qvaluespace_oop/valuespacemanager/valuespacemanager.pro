SOURCES += main.cpp
TARGET = valuespacemanager
INCLUDEPATH += ../../../../context
QT = core

CONFIG+= testcase

include(../../../../common.pri)

LIBS += -lQtContextFramework
