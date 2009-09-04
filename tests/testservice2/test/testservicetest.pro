TEMPLATE = app
TARGET = testservicetest
QT -= gui
QT+=testlib 
INCLUDEPATH += ../../../../inc ..

# Input 
SOURCES += testservicetest.cpp ../testservice.cpp ../testserviceiterable.cpp ../userdefinedclass.cpp
HEADERS += ../testservice.h ../testserviceiterable.h ../userdefinedclass.h

CONFIG(release):TESTSVC_TEST_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Release
CONFIG(debug):TESTSVC_TEST_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Debug

OBJECTS_DIR = $$TESTSVC_TEST_OUTPUT_DIR/testservicetest/tmp
DESTDIR = $$TESTSVC_TEST_OUTPUT_DIR/bin
MOC_DIR=$$TESTSVC_TEST_OUTPUT_DIR/testservicetest/tmp
RCC_DIR=$$TESTSVC_TEST_OUTPUT_DIR/testservicetest/tmp
TEMPDIR=$$TESTSVC_TEST_OUTPUT_DIR/testservicetest/build

symbian { 
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
    LIBS            += -lefsrv -lPlatformEnv
    INCLUDEPATH     += ${EPOCROOT}/epoc32/include/osextensions
}