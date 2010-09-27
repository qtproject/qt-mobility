load(qttest_p4)
include (../../../common.pri)
QT += declarative
CONFIG += mobility
MOBILITY += feedback

SOURCES += tst_qdeclarativefeedback.cpp

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += testcase
