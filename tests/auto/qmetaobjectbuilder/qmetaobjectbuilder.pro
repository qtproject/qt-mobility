TARGET = tst_qmetaobjectbuilder
INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/serviceframework/ipc

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_qmetaobjectbuilder.cpp

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

    addFiles.sources = ../../testservice2/xml/testserviceplugin.xml \
                       ../../sampleserviceplugin/xml/sampleservice.xml \
                       ../../sampleserviceplugin2/xml/sampleservice2.xml
    addFiles.path = plugins/xmldata
    DEPLOYMENT += addFiles
}
