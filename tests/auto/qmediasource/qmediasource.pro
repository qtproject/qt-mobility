TARGET = tst_qmediasource
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediasource.cpp

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

