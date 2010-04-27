
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
    
    contains(S60_VERSION, 3.1) {
        
        # In S60 3.1 we need to use TMobilePhoneBookInfoV1 instead of TMobilePhoneBookInfoV5.
        # Note: Etel testsserver uses V5 always.
        !contains(DEFINES, SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER) {
            	DEFINES += SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
        }
    }

    # Enable checking the existence of a contact before trying to remove it.
    # This is needed because a contact backend is supposed to give an error
    # when trying to remove a nonexistent contact, but SIM store does not
    # give an error in such situations.
    # In S60 3.1 devices the consequences are even worse; the device will
    # reboot when trying to remove several nonexistent contacts in a row.
    # The first remove operation will succeed but right after the second
    # remove operation has completed the device reboots.
    DEFINES += SYMBIANSIM_BACKEND_CHECK_BEFORE_REMOVE
    
    # In pre 10.1 platforms we need a small delay between requests to prevent
    # S60 3.2 devices from rebooting and S60 5.0 devices from reporting a
    # server busy error. Not sure if this is really needed for S60 3.1 but
    # it does not hurt. 
    contains(S60_VERSION, 3.1) | contains(S60_VERSION, 3.2) | contains(S60_VERSION, 5.0) {
       DEFINES += SYMBIANSIM_BACKEND_USE_DELAY
    }     
}
