TARGET=tst_qabstractsecuritysession
INCLUDEPATH += ../../../serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

HEADERS += ../qsfwtestutil.h
SOURCES += tst_qabstractsecuritysession.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian|wince* {
    
    symbian {
        TARGET.CAPABILITY = ALL -TCB
        LIBS += -lefsrv
    }

    wince* {
        SFWTEST_PLUGIN_DEPLOY.sources = \
                $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/tst_sfw_sampleserviceplugin.dll \
                $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/tst_sfw_sampleserviceplugin2.dll \
                $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/tst_sfw_testservice2plugin.dll
        SFWTEST_PLUGIN_DEPLOY.path = .
        DEPLOYMENT += SFWTEST_PLUGIN_DEPLOY
        DEPLOYMENT_PLUGIN += qsqlite
    }
    
    addFiles.sources = ../../testservice2/xml/testserviceplugin.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles
}
