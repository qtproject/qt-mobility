######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################

CONFIG += debug_and_release build_all

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

#test whether we have a unit test
#assumption is that tests always start with tst_<name>
substr = $$find(TARGET, tst_.*)
isEmpty(substr) {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
} else {
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against library that we test
}


# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR

