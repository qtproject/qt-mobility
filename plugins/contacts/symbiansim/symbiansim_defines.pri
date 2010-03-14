
symbian: {
    # Uncomment this to use Etel test server (instead of real Etel APIs)
    #DEFINES += SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER

    # add either real or test libraries for Etel
    contains(DEFINES, SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER): {
        message("Using Etel Test Server (not real Etel)")
        INCLUDEPATH +=$${EPOCROOT}epoc32/include/internal
        LIBS += -leteltestserverclient
    } else {
        message("Using real Etel APIs")
        LIBS += -letel \
                -letelmm
    }
    
    # In S60 3.1 we need to use TMobilePhoneBookInfoV1 instead of TMobilePhoneBookInfoV5
    contains(S60_VERSION, 3.1) {
        DEFINES += SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
    }    
}
