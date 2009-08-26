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

# Make sure this goes everywhere we need it
symbian: load(data_caging_paths)

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

# Which backend we're building
## memory / invalid / symbian / windows / vcard / kabc etc.
CONTACTS_BACKENDS = memory invalid
wince*: CONTACTS_BACKENDS += wince
symbian: CONTACTS_BACKENDS += symbian

symbian:CONTACTS_BACKENDS += symbians60
wince*:CONTACTS_BACKENDS += wince

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

#test whether we have a unit test
!testcase {
    # Normal library/plugin/example code
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin:!testplugin {
        DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin
    } else {
        # This is where plugins get built
        testplugin:DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts
        !testplugin:DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts

        # And since we're a plugin, add the base lib path to the lib dirs
        LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against base dir as well
    }
    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc

} else {
    # Unit test code (no plugins! test plugins are just normal plugins installed elsewhere)
    QT *= testlib
    CONFIG += console
    CONFIG -= app_bundle
    OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
    DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin
    MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc

    LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against library that we test
}

INCLUDEPATH *= $$MOC_DIR
INCLUDEPATH *= $$RCC_DIR

# Add files for deployment
wince* {
    # Main library
    CONTACTS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/QtContacts.dll
    CONTACTS_DEPLOYMENT.path = /Windows

    # Plugins
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/*.dll
    CONTACTS_PLUGINS_DEPLOYMENT.path = /resource/qt/plugins/contacts

    DEPLOYMENT += CONTACTS_DEPLOYMENT CONTACTS_PLUGINS_DEPLOYMENT
}
symbian {
    # Main library
    CONTACTS_DEPLOYMENT.sources = QtContacts.dll
    CONTACTS_DEPLOYMENT.path = \sys\bin

    # Engine plugins should be installed in their own .pro
    DEPLOYMENT += CONTACTS_DEPLOYMENT
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . \
    $$SOURCE_DIR \
    $$SOURCE_DIR/contacts \
    $$SOURCE_DIR/contacts/details \
    $$SOURCE_DIR/contacts/engines \
    $$SOURCE_DIR/contacts/filters \
    $$SOURCE_DIR/contacts/requests

