symbian: {
    
	IS_SYMBIAN_3 = no
	exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
	    message(Building Symbian Plugin on S^3 or later Symbian Platform)
	    IS_SYMBIAN_3 = yes
	
	} else {
		message(Building Symbian Plugin on Pre S^3 Symbian Platform)
	}

	contains(IS_SYMBIAN_3, no) {
	   # In pre 9.2 platforms Symbian Landmarks Apis don't support OR Composition for searching
	   # This macro will avoid usage of CPosLmCompositeCriteria::ECompositionOR.
	   DEFINES += SYMBIAN_OR_COMPOSITION_RESTRICTED
	}

}
