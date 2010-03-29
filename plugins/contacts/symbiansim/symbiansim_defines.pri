
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
        
        # S60 3.1 device will reboot when removing several nonexisting contacts in
        # sequence. The first remove operation will succeed but right after the second
        # remove operation has completed the device reboots. To prevent rebooting
        # we read the reserved slots before and really remove those contacts that
        # really exist.
        DEFINES += SYMBIANSIM_BACKEND_CHECK_BEFORE_REMOVE
    }
    
    # In pre 10.1 platforms we need a small delay between requests to prevent
    # S60 3.2 devices from rebooting and S60 5.0 devices from reporting a
    # server busy error. Not sure if this is really needed for S60 3.1 but
    # it does not hurt. 
    contains(S60_VERSION, 3.1) | contains(S60_VERSION, 3.2) | contains(S60_VERSION, 5.0) {
       DEFINES += SYMBIANSIM_BACKEND_USE_DELAY
    }     
}
