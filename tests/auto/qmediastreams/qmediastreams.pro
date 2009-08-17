TARGET = tst_qmediastreams

CONFIG += testcase

SOURCES += tst_qmediastreams.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

