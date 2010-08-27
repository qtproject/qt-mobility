symbian: {
    
    contains(S60_VERSION, 3.1) | contains(S60_VERSION, 3.2) | contains(S60_VERSION, 5.0) {
    
       # In pre 9.2 platforms Symbian Landmarks Apis don't support OR Composition for searching
       # This macro will avoid usage of CPosLmCompositeCriteria::ECompositionOR.
       DEFINES += SYMBIAN_OR_COMPOSITION_RESTRICTED
       
    }     
}
