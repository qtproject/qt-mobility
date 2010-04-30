#
# Code for making sure our test plugins go to the right places
# (by default, these are private plugins for cross compilation platforms)
#
# There is typically a completely separate folder for each unit test
# on these platforms, so installing plugins to those dirs is acceptable.
#
# On other platforms, all tests are built and live in a single place
# so we put the test plugins into a separate dir, since we don't want
# these test plugins available for all unit tests.
#
symbian{
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB

    testplugin.sources = $${TARGET}.dll
    testplugin.path = plugins\contacts
    DEPLOYMENT += testplugin

    target.path = plugins\contacts
    INSTALLS += target
}

wince* {
    testplugin.sources = $${TARGET}.dll
    testplugin.path = ./plugins
    DEPLOYMENT += testplugin

    target.path = ./plugins
    INSTALLS += target
}

# Add this too
CONFIG += mobility
MOBILITY = contacts


