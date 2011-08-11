TARGET = tst_qmetaobjectbuilder
INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/serviceframework/ipc
include(../../../src/serviceframework/ipc/metaobjectbuilder.pri)
INCLUDEPATH += ../../../src/serviceframework/$$OBJECTBUILDER_INCLUDEPATH

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 

contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 8) {
    SOURCES += tst_qmetaobjectbuilder_47.cpp
} else {
    SOURCES += tst_qmetaobjectbuilder.cpp
}

CONFIG += mobility
MOBILITY = serviceframework

symbian|wince* {
    symbian {
        TARGET.CAPABILITY = ALL -TCB
        LIBS += -lefsrv
    }
    wince* {
        SFWTEST_PLUGIN_DEPLOY.sources = \
                $$OUTPUT_DIR/build/tests/bin/plugins/tst_sfw_sampleserviceplugin.dll \
                $$OUTPUT_DIR/build/tests/bin/plugins/tst_sfw_sampleserviceplugin2.dll \
                $$OUTPUT_DIR/build/tests/bin/plugins/tst_sfw_testservice2plugin.dll
        SFWTEST_PLUGIN_DEPLOY.path = plugins
        DEPLOYMENT += SFWTEST_PLUGIN_DEPLOY
        DEPLOYMENT_PLUGIN += qsqlite
    }

    addFiles.sources = ../../tools/testservice2/xml/testserviceplugin.xml \
                       ../../tools/sampleserviceplugin/xml/sampleservice.xml \
                       ../../tools/sampleserviceplugin2/xml/sampleservice2.xml
    addFiles.path = plugins/xmldata
    DEPLOYMENT += addFiles
}
