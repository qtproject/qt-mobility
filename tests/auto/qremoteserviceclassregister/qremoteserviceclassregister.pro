TARGET = tst_qremoteserviceclassregister
INCLUDEPATH += ../../../src/serviceframework

CONFIG+=testcase

QT = core sql

include(../../../common.pri)

# Input 
HEADERS += 
SOURCES += tst_qremoteserviceclassregister.cpp \

CONFIG += mobility
MOBILITY = serviceframework

