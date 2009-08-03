TARGET = tst_qmediaplaylist

CONFIG += testcase

SOURCES += tst_qmediaplaylist.cpp

include (../../../common.pri)
INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

