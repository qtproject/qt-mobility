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

mac {
    contains(QT_CONFIG, qt_framework):contains(TEMPLATE, lib) {
        #MacOSX always builds debug and release libs when using mac framework
        CONFIG+=$$WAS_IN_DEBUG
        CONFIG += debug_and_release build_all
    } else {
        !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
            CONFIG -= debug_and_release debug release
            contains(QT_CONFIG,debug): CONFIG+=debug
            contains(QT_CONFIG,release): CONFIG+=release
        }
    }
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

CONFIG(debug, debug|release) {
    SUBDIRPART=Debug
} else {
    SUBDIRPART=Release
}


#test whether we have a unit test
!testcase {
    OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
    !plugin {
        contains(TEMPLATE, lib):DESTDIR = $$OUTPUT_DIR/lib
        else:DESTDIR = $$OUTPUT_DIR/bin
    } else {
        testplugin:DESTDIR = $$OUTPUT_DIR/build/tests/bin/plugins/$$PLUGIN_TYPE
        !testplugin:DESTDIR = $$OUTPUT_DIR/plugins/$$PLUGIN_TYPE
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
    CONTACTS_PLUGINS_DEPLOYMENT.sources = $$OUTPUT_DIR/plugins/contacts/*.dll
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
    
    TARGET.CAPABILITY = ALL -TCB
}

# Add the output dirs to the link path too
mac:contains(QT_CONFIG,qt_framework) {
    #add framework option
    contains(TEMPLATE, app)|contains(CONFIG,plugin):LIBS+=-F$$OUTPUT_DIR/lib
}
LIBS += -L$$OUTPUT_DIR/lib

DEPENDPATH += . $$SOURCE_DIR
#INCLUDEPATH += $$SOURCE_DIR

contains(QT_CONFIG, multimedia) {
    QT += multimedia
} else {
    DEFINES *= QT_NO_MULTIMEDIA
}

# Messaging specific:
DEFINES+=QMESSAGING_OPTIONAL_FOLDER

