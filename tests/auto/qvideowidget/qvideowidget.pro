TARGET = tst_qvideowidget
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qvideowidget.cpp

INCLUDEPATH += ../../../multimedia

LIBS += -lQtMedia
