TARGET = tst_qservicedeletion
INCLUDEPATH += ../../../src/serviceframework

CONFIG += testcase

QT = core sql

include(../../../common.pri)

# Input 
HEADERS += ../qsfwtestutil.h
SOURCES += \
           ../qsfwtestutil.cpp \
    tst_qservicedeletion.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian|wince* {
    symbian {
        TARGET.CAPABILITY = ALL -TCB
        LIBS += -lefsrv
        contains(S60_VERSION, 5.2)|contains(MOBILITY_SD_MCL_BUILD, yes){
            DEFINES += SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD
        }
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
}

xml.path = $$DESTDIR/xmldata
xml.files = testdeletion.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
