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

# Helper function.  This should move to a .prf file
defineReplace(mobilityDeployFilename) {
   unset(MOBILITY_DEPLOY_NAME)
   MOBILITY_DEPLOY_NAME = $$1
   CONFIG(debug, debug|release): debug_and_release: {
      mac:RET = $$member(MOBILITY_DEPLOY_NAME, 0)_debug
      else:win32:RET = $$member(MOBILITY_DEPLOY_NAME, 0)d
   }
   isEmpty(RET):RET = $$MOBILITY_DEPLOY_NAME
   return($$RET)
}

#
# This function (qtAddLibrary) is broken pre 4.6 for non debug_and_release
# builds, since the name decoration is inconsistent between qtLibraryTarget
# and qtAddLibrary
#
defineTest(qtAddLibraryFixMe) {
    INCLUDEPATH -= $$QMAKE_INCDIR_QT/$$1
    INCLUDEPATH = $$QMAKE_INCDIR_QT/$$1 $$INCLUDEPATH

    LIB_NAME = $$1
    unset(LINKAGE)
    mac {
       CONFIG(qt_framework, qt_framework|qt_no_framework) { #forced
          QMAKE_FRAMEWORKPATH *= $${QMAKE_LIBDIR_QT}
          FRAMEWORK_INCLUDE = $$QMAKE_LIBDIR_QT/$${LIB_NAME}.framework/Headers
          !qt_no_framework_direct_includes:exists($$FRAMEWORK_INCLUDE) {
             INCLUDEPATH -= $$FRAMEWORK_INCLUDE
             INCLUDEPATH = $$FRAMEWORK_INCLUDE $$INCLUDEPATH
           }
           LINKAGE = -framework $${LIB_NAME}$${QT_LIBINFIX}
        } else:!qt_no_framework { #detection
           for(frmwrk_dir, $$list($$QMAKE_LIBDIR_QT $$QMAKE_LIBDIR $$(DYLD_FRAMEWORK_PATH) /Library/Frameworks)) {
              exists($${frmwrk_dir}/$${LIB_NAME}.framework) {
                QMAKE_FRAMEWORKPATH *= $${frmwrk_dir}
                FRAMEWORK_INCLUDE = $$frmwrk_dir/$${LIB_NAME}.framework/Headers
                !qt_no_framework_direct_includes:exists($$FRAMEWORK_INCLUDE) {
                  INCLUDEPATH -= $$FRAMEWORK_INCLUDE
                  INCLUDEPATH = $$FRAMEWORK_INCLUDE $$INCLUDEPATH
                }
                LINKAGE = -framework $${LIB_NAME}
                break()
              }
           }
       }
    }
    symbian*:isEqual(LIB_NAME, QtGui) {
        # Needed for #include <QtGui> because qs60mainapplication.h includes aknapp.h
        INCLUDEPATH *= $$MW_LAYER_SYSTEMINCLUDE
    }
    isEmpty(LINKAGE) {
       if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
           win32:LINKAGE = -l$${LIB_NAME}$${QT_LIBINFIX}d
           mac:LINKAGE = -l$${LIB_NAME}$${QT_LIBINFIX}_debug
       }
       isEmpty(LINKAGE):LINKAGE = -l$${LIB_NAME}$${QT_LIBINFIX}
    }
    !isEmpty(QMAKE_LSB) {
        QMAKE_LFLAGS *= --lsb-libpath=$$$$QMAKE_LIBDIR_QT
        QMAKE_LFLAGS *= -L/opt/lsb/lib
        QMAKE_LFLAGS *= --lsb-shared-libs=$${LIB_NAME}$${QT_LIBINFIX}
    }
    LIBS += $$LINKAGE
    export(LIBS)
    export(INCLUDEPATH)
    export(QMAKE_FRAMEWORKPATH)
    export(QMAKE_LFLAGS)
    return(true)
}

# Make sure this goes everywhere we need it
symbian: load(data_caging_paths)

# For symbian, we are not attempting to freeze APIs yet.
symbian:MMP_RULES += "EXPORTUNFROZEN"

CONFIG(debug, debug|release) {
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
    CONTACTS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/$$mobilityDeployFilename(QtContacts).dll
    CONTACTS_DEPLOYMENT.path = /Windows

    # Plugins
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/build/$$SUBDIRPART/bin/plugins/contacts/*.dll
    CONTACTS_PLUGINS_DEPLOYMENT.path = plugins/contacts

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

