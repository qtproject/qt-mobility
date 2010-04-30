TARGET = tst_qmediapluginloader
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

SOURCES += tst_qmediapluginloader.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

wince* {
    PLUGIN_DEPLOY.sources = $$OUTPUT_DIR/plugins/mediaservice/*.dll
    PLUGIN_DEPLOY.path = mediaservice
    DEPLOYMENT += PLUGIN_DEPLOY
}

