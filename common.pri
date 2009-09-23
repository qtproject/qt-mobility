######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################


CONFIG(debug, debug|release) {
    WAS_IN_DEBUG=debug
} else {
    WAS_IN_DEBUG=release
}

include(config.pri)


#MacOSX always builds debug and release libs
mac:contains(TEMPLATE, lib) {
    CONFIG+=$$WAS_IN_DEBUG
    CONFIG += debug_and_release build_all
}

# For symbian, we are not attempting to freeze APIs yet.
symbian:MMP_RULES += "EXPORTUNFROZEN"

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

CONFIG(debug, debug|release) {
    SUBDIRPART=Debug
} else {
    SUBDIRPART=Release
}



#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    contains(TEMPLATE, lib):DESTDIR = $$OUTPUT_DIR/lib
    else:DESTDIR = $$OUTPUT_DIR/bin
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
} else {
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/tests/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/ui
    LIBS += -L$$OUTPUT_DIR/lib  #link against library that we test
    symbian {
        #The default include path doesn't include MOC_DIR on symbian
        INCLUDEPATH += $$MOC_DIR
    }
}

wince* {
    BEARERLIB.sources = $$OUTPUT_DIR/lib/bearer.dll
    BEARERLIB.path = .
    DEPLOYMENT += BEARERLIB
}

# Add the output dirs to the link path too
LIBS += -L$$OUTPUT_DIR/lib

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . $$SOURCE_DIR
