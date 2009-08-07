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
} else {
    SUBDIRPART = Release
}

# Figure out the root of where stuff should go (this could be done via configure)
example|plugin {
    OUTPUT_DIR = $$OUT_PWD/../..
    SOURCE_DIR = $$PWD/../..
} else: testcase {
    OUTPUT_DIR = $$OUT_PWD/../../..
    SOURCE_DIR = $$PWD/../../..
} else {
    OUTPUT_DIR = $$OUT_PWD/..
    SOURCE_DIR = $$PWD/..
}


#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
} else {
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/ui
    LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against library that we test
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
#For some reason the default include path doesn't include MOC_DIR on symbian
symbian {
	INCLUDEPATH += $$MOC_DIR
}

for(p, QMAKE_INCDIR_QT) {
    exists("$${p}/QtMultimedia/qabstractvideosurface.h"): CONFIG *= videosurface
}
!contains(CONFIG, videosurface): DEFINES += QT_NO_VIDEOSURFACE

plugin: !isEmpty(PLUGIN_SUBDIR): DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/$$PLUGIN_SUBDIR
