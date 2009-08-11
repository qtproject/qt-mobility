TARGET = tst_qmediarecorder

CONFIG += testcase

SOURCES += tst_qmediarecorder.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

