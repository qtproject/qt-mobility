# Code for making sure our test plugins go to the right places
# (by default, these are private plugins)
symbian{
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL

    testplugin.sources = $${TARGET}.dll
    testplugin.path = plugins\contacts
    DEPLOYMENT += testplugin

    target.path = plugins\contacts
    INSTALLS += target
}

wince {
    testplugin.sources = $${TARGET}.dll
    testplugin.path = ./plugins
    DEPLOYMENT += testplugin

    target.path = ./plugins
    INSTALLS += target
}

