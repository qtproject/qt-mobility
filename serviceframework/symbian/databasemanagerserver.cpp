#include "databasemanagerserver.h"
#include "clientservercommon.h"
#include "databasemanagersession.h"

CDatabaseManagerServer::CDatabaseManagerServer()
    : CServer2(EPriorityNormal)
    {
    }

CSession2* CDatabaseManagerServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
        if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber, 
                KServerMinorVersionNumber, KServerBuildVersionNumber), aVersion))
            {
            User::Leave(KErrNotSupported);
            }

        return CDatabaseManagerServerSession::NewL();
    }

void CDatabaseManagerServer::PanicServer(TDatabaseManagerSerververPanic aPanic)
    {
    _LIT(KTxtServerPanic,"Database manager server panic");
    User::Panic(KTxtServerPanic, aPanic);
    }

// End of File
