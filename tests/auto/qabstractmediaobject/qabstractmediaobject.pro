TARGET = tst_qabstractmediaobject
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qabstractmediaobject.cpp

INCLUDEPATH += ../../../multimedia

LIBS += -lQtMedia
