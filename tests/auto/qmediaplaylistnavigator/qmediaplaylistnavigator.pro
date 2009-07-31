TARGET = tst_qmediaplaylistnavigator

CONFIG += testcase

SOURCES += tst_qmediaplaylistnavigator.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

