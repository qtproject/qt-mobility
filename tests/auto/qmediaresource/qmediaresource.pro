TARGET = tst_qmediaresource
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediaresource.cpp

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

