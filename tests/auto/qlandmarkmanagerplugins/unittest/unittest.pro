QT += testlib
TEMPLATE=app
TARGET=tst_qlandmarkmanagerplugins
CONFIG+=testcase

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location


CONFIG += mobility
MOBILITY = location
SOURCES  += tst_qlandmarkmanagerplugins.cpp

!symbian {
LIBS += -L../../../../build/tests/bin/plugins/landmarks/ -llandmarks_testdummystatic

TESTDLLS = \
    $$mobilityDeployFilename(landmarks_testdummystatic)
}

# App local deployment
symbian|wince* {
    symbian:QLANDMARKMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, ".dll ", " ", ".dll")
    wince*:QLANDMARKMANAGER_PLUGINS_DEPLOY.sources = $$join(TESTDLLS, ".dll $$OUTPUT_DIR/build/tests/bin/plugins/landmarks/", "$$OUTPUT_DIR/build/tests/bin/plugins/landmarks/", ".dll")
    QLANDMARKMANAGER_PLUGINS_DEPLOY.path = ./plugins/landmarks
    DEPLOYMENT += QLANDMARKMANAGER_PLUGINS_DEPLOY
}

# all else, we need to copy and install the DLLs to the right place (in a subdir for this test)

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
}
