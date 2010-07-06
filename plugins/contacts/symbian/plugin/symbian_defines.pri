
symbian: {
    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        exists($${EPOCROOT}epoc32/release/winscw/udeb/VPbkEng.dll) \
        | exists($${EPOCROOT}epoc32/release/armv5/urel/VPbkEng.dll) {
            message("S^3 platform")
        } else {
            message("S^4 or later platform")
            DEFINES += SYMBIAN_BACKEND_USE_SQLITE
            
            # This will enable signals to be emitted sychronously with every
            # operation instead of them beeing emitted when database event observer
            # interface if called (HandleDatabaseEventL). This is an optimization
            # for S^4 platform. However enabling this will cause some problems
            # with signals concerning adding/removing groups/group participants. 
            DEFINES += SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
        }
    }
    
    contains(S60_VERSION, 3.2) {
    	DEFINES += SYMBIAN_BACKEND_S60_VERSION_32
    }
}
