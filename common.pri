######################################################################
#
# Mobility API project - common QMake settings
#
######################################################################

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

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

# Make sure this goes everywhere we need it
symbian: load(data_caging_paths)

# For symbian, we are not attempting to freeze APIs yet.
symbian:MMP_RULES += "EXPORTUNFROZEN"

# Which backend we're building
## memory / invalid / symbian / windows / vcard / kabc etc.
CONTACTS_BACKENDS += memory invalid
wince*:CONTACTS_BACKENDS += wince

symbian:CONTACTS_BACKENDS += symbian

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin:!testplugin {
        contains(TEMPLATE, lib):DESTDIR = $$OUTPUT_DIR/lib
        else:DESTDIR = $$OUTPUT_DIR/bin
    } else {
        ### plugins get installed to the plugins directory
        testplugin:DESTDIR = $$OUTPUT_DIR/build/tests/bin/plugins/contacts
        !testplugin:DESTDIR = $$OUTPUT_DIR/build/bin/plugins/contacts

        # And since we're a plugin, add the base lib path to the lib dirs
        testplugin:LIBS += -L$$OUTPUT_DIR/build/tests/bin  #link against base dir as well
        !testplugin:LIBS += -L$$OUTPUT_DIR/build/bin       #link against base dir as well
    }

    MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
    RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
    UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
} else {
    # Unit test code (no plugins! test plugins are just normal plugins installed elsewhere)
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
    ### Bearer Management
    BEARERLIB.sources = $$OUTPUT_DIR/lib/bearer.dll
    BEARERLIB.path = .
    DEPLOYMENT += BEARERLIB

    ### Contacts
    # Main library
    CONTACTS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/QtContacts.dll
    CONTACTS_DEPLOYMENT.path = /Windows

    # Plugins
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts/*.dll
    CONTACTS_PLUGINS_DEPLOYMENT.path = plugins/contacts

    DEPLOYMENT += CONTACTS_DEPLOYMENT CONTACTS_PLUGINS_DEPLOYMENT
}
symbian {
    ### Contacts
    # Main library
    CONTACTS_DEPLOYMENT.sources = QtContacts.dll
    CONTACTS_DEPLOYMENT.path = \sys\bin

    # Engine plugins should be installed in their own .pro
    DEPLOYMENT += CONTACTS_DEPLOYMENT
}

# Add the output dirs to the link path too
LIBS += -L$$OUTPUT_DIR/lib

DEPENDPATH += . $$SOURCE_DIR
INCLUDEPATH += . \
    $$SOURCE_DIR \
    $$SOURCE_DIR/contacts \
    $$SOURCE_DIR/contacts/details \
    $$SOURCE_DIR/contacts/engines \
    $$SOURCE_DIR/contacts/filters \
    $$SOURCE_DIR/contacts/requests
