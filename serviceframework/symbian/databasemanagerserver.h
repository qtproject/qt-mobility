#ifndef CDATABASEMANAGERSERVER_H_
#define CDATABASEMANAGERSERVER_H_

#include <e32base.h>
#include "databasemanagerserver.pan"
#include "databasemanagerserver_global.h"

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

class DATABASEMANAGERSERVER_EXPORT CDatabaseManagerServer : public CServer2
    {
    public:
        CDatabaseManagerServer();
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    public:
        static void PanicServer(TDatabaseManagerSerververPanic aPanic);
    };

#endif

// End of File
