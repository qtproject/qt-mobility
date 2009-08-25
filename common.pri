######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################

CONFIG += debug_and_release build_all

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

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
    CONFIG += console
} else {
    SUBDIRPART = Release
}

# Which backend we're building
## memory / invalid / symbian / windows / vcard / kabc etc.
CONTACTS_BACKENDS = memory invalid

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

# Default plugin subdirectory
isEmpty(PLUGIN_SUBDIR): PLUGIN_SUBDIR=plugins

#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin {
        DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin
    } else {
        DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/$$PLUGIN_SUBDIR
        LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against base dir as well
    }
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc

} else {
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    !plugin {
        DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin
    } else {
        DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/$$PLUGIN_SUBDIR
    }
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against library that we test
    INCLUDEPATH *= $$MOC_DIR
    INCLUDEPATH *= $$RCC_DIR

    # Where test plugins go.
    TEST_PLUGIN_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/$$PLUGIN_SUBDIR
}

# Where normal plugins go
PLUGIN_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/$$PLUGIN_SUBDIR

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCEDIR

# Messaging specific:
DEFINES+=QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER

