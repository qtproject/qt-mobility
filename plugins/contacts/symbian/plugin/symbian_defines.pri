
symbian: {
    contains(symbiancntmodelv2_enabled, yes) {
        message("S^3 PS3 or later platform")
        DEFINES += SYMBIAN_BACKEND_USE_SQLITE
            
        # This will enable signals to be emitted sychronously with every
        # operation instead of them beeing emitted when database event observer
        # interface if called (HandleDatabaseEventL). This is an optimization
        # for S^3 PS3+ platform. However enabling this will cause some problems
        # with signals concerning adding/removing groups/group participants. 
        DEFINES += SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
    } else {
        message("S^3 PS2 or older platform")
    }
    
    contains(S60_VERSION, 3.2) {
    	DEFINES += SYMBIAN_BACKEND_S60_VERSION_32
    }
}
