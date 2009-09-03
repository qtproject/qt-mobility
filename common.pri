######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################

#CONFIG += debug_and_release build_all
include(config.pri)

# On win32 and mac, debug and release libraries are named differently.
# We must follow the debug and release settings Qt was compiled with:
# build debug iff Qt built debug, build release iff Qt built release.
win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

SUBDIRPART = .objects
#CONFIG(debug, debug|release) {
#    SUBDIRPART = Debug
#} else {
#    SUBDIRPART = Release
#}

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
isEmpty(target.path) {
    INSTALL_DIR = $$PWD
} else {
    INSTALL_DIR = $${QT_MOBILITY_PREFIX}
}
SOURCE_DIR = $$PWD

#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    DESTDIR = $$INSTALL_DIR/bin
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
} else {
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$INSTALL_DIR/tests/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/ui
    LIBS += -L$$INSTALL_DIR/bin  #link against library that we test
    symbian {
        #The default include path doesn't include MOC_DIR on symbian
        INCLUDEPATH += $$MOC_DIR
    }
}

wince* {
    BEARERLIB.sources = $$INSTALL_DIR/bin/bearer.dll
    BEARERLIB.path = .
    DEPLOYMENT += BEARERLIB
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
