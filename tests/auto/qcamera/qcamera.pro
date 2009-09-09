TARGET = tst_qcamera

CONFIG += testcase

SOURCES += tst_qcamera.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia

LIBS += -lQtMedia

