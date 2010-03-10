TARGET = tst_qmediaplayer_s60
INCLUDEPATH += ../../../../src/multimedia \
               ../../../../include 
CONFIG += testcase

QT += testlib

SOURCES += tst_qmediaplayer_s60.cpp

include (../../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

testFiles.sources = testfiles\*
testFiles.path = /Data/testfiles
DEPLOYMENT += testFiles