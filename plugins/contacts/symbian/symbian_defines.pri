
symbian: {
    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        exists($${EPOCROOT}epoc32/release/winscw/udeb/VPbkEng.dll) \
        | exists($${EPOCROOT}epoc32/release/armv5/urel/VPbkEng.dll) {
            message("TB 9.2 platform")
        } else {
            message("TB 10.1 or later platform")
            DEFINES += SYMBIAN_BACKEND_USE_SQLITE
        }
    }
    
    contains(S60_VERSION, 3.2) {
    	DEFINES += SYMBIAN_BACKEND_S60_VERSION_32
    }
}