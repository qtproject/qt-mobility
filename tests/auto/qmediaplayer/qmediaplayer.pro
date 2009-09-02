TARGET = tst_qmediaplayer

CONFIG += testcase

SOURCES += tst_qmediaplayer.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia

LIBS += -lQtMedia

