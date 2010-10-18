load(qttest_p4)
include (../../../common.pri)
QT += declarative
CONFIG += mobility
MOBILITY += feedback

SOURCES += tst_qdeclarativefeedback.cpp

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/feedback

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += testcase

OTHER_FILES += \
    data/hapticseffect.qml \
    data/fileeffect.qml \
    data/actuator.qml \
    data/themeeffect.qml \
    data/themeeffect2.qml \
    data/themeeffect3.qml
