TARGET = tst_qradioplayer

CONFIG += testcase

SOURCES += tst_qradioplayer.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

