
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
    
    # Enable checking the existence of a contact before trying to remove it.
    # This is needed because a contact backend is supposed to give an error
    # when trying to remove a nonexistent contact, but SIM store does not
    # give an error in such situations.
    DEFINES += SYMBIANSIM_BACKEND_CHECK_BEFORE_REMOVE
    
    contains(S60_VERSION, 5.0) {
       # We need a small delay between requests to prevent S60 5.0 devices
       # from reporting a server busy error. 
       DEFINES += SYMBIANSIM_BACKEND_USE_DELAY
       
       # In 5.0 platform we need to check extra detail support 
       # (nickname/additional number/email) by trying to write them to sim card. 
       # This is because when using RMobilePhoneStore::GetInfo() it does not
       # report them correctly. 
       # TODO: Check it in 9.2, there is another API for checking this but
       # it is not public - could it be used?
       DEFINES += SYMBIANSIM_BACKEND_TEST_EXTRADETAILS
    }     
}
