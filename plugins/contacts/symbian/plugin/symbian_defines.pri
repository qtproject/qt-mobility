symbian: {
    exists($${EPOCROOT}epoc32/data/z/system/install/productid_helen.sis) | exists($${EPOCROOT}epoc32/rom/config/ncp110) {
        DEFINES += SYMBIAN_BACKEND_USE_CNTMODEL_V2
            
        # This will enable signals to be emitted sychronously with every
        # operation instead of them beeing emitted when database event observer
        # interface if called (HandleDatabaseEventL). This is an optimization
        # for S^3 1.11+ platform.
        DEFINES += SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
        
    }

    contains(S60_VERSION, 5.0) {
	      DEFINES += SYMBIAN_USE_SMALL_THUMBNAILS
    } 
}
