TARGET = tst_qmediametadata

CONFIG += testcase

SOURCES += tst_qmediametadata.cpp

include (../../../common.pri)

INCLUDEPATH += ../../../multimedia ../../../multimedia/endpoints

LIBS += -lQtMedia

