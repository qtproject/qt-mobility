//  Include Files  

#include <e32base.h>
#include <e32std.h>

#include "CETelServer.h"

//  Exported Functions

GLDEF_C TInt E32Main()
    {
    return CETelServer::ThreadFunction();
    }


// End of file