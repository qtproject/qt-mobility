TARGET = tst_qmediaserviceprovider
INCLUDEPATH += ../../../multimedia ../../..
CONFIG += testcase

SOURCES += tst_qmediaserviceprovider.cpp

include (../../../common.pri)

qtAddLibrary(QtMedia)

