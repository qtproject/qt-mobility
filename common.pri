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

# After fixing that part up, we might need to re-set TARGET
contains(TEMPLATE, .*lib):TARGET=$$qtLibraryTarget($$TARGET)

# Helper function.  This should move to a .prf file
defineReplace(mobilityDeployFilename) {
   unset(MOBILITY_DEPLOY_NAME)
   MOBILITY_DEPLOY_NAME = $$1
   CONFIG(debug, debug|release): {
      mac:RET = $$member(MOBILITY_DEPLOY_NAME, 0)_debug
      else:win32:RET = $$member(MOBILITY_DEPLOY_NAME, 0)d
   }
   isEmpty(RET):RET = $$MOBILITY_DEPLOY_NAME
   return($$RET)
}

# Make sure this goes everywhere we need it
symbian: load(data_caging_paths)

# All frameworks (debug and release) go in the same directory
mac:contains(TEMPLATE,.*lib):contains(QT_CONFIG,qt_framework):contains(TARGET,Qt.*) {
    SUBDIRPART = Release
} else:CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

# Which backend we're building
## memory / invalid / symbian / windows / vcard / kabc etc.
CONTACTS_BACKENDS += memory invalid
wince*:CONTACTS_BACKENDS += wince

symbian:CONTACTS_BACKENDS += symbian

# Figure out the root of where stuff should go (this could be done via configure)
OUTPUT_DIR = $$PWD
SOURCE_DIR = $$PWD

mac:MOBILITY_FRAMEWORK_PATH=$$OUTPUT_DIR/build/Release/bin

#test whether we have a unit test
!testcase {
    # Normal library/plugin/example code
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin:!testplugin {
        DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin
        mac:contains(QT_CONFIG,qt_framework):LIBS += -F$$MOBILITY_FRAMEWORK_PATH
    } else {
        # This is where plugins get built
        testplugin:DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts
        !testplugin:DESTDIR = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts

        # And since we're a plugin, add the base lib path to the lib dirs
        LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin
        mac:contains(QT_CONFIG,qt_framework):LIBS += -F$$MOBILITY_FRAMEWORK_PATH
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

    LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin
    mac:contains(QT_CONFIG,qt_framework):LIBS += -F$$MOBILITY_FRAMEWORK_PATH
}

INCLUDEPATH *= $$MOC_DIR
INCLUDEPATH *= $$RCC_DIR

# Add files for deployment
wince* {
    # Main library
    CONTACTS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/$$mobilityDeployFilename(QtContacts).dll
    CONTACTS_DEPLOYMENT.path = /Windows

    # Plugins
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts/*.dll
    CONTACTS_PLUGINS_DEPLOYMENT.path = plugins/contacts

    DEPLOYMENT += CONTACTS_DEPLOYMENT CONTACTS_PLUGINS_DEPLOYMENT
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

