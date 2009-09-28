TARGET = tst_qmediaimageviewer
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qmediaimageviewer.cpp

LIBS += -lQtMedia

!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG

wince*|symbian {
    DEFINES += TESTDATA_DIR=\\\"./tst_qmediaimageviewer_images\\\"

    addFiles.sources = images/*
    addFiles.path = tst_qmediaimageviewer_images
    DEPLOYMENT += addFiles
} else {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/images\\\"
}
