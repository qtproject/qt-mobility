#ifndef CLIENTSERVERCOMMON_H_
#define CLIENTSERVERCOMMON_H_

#include <e32base.h>

_LIT(KDatabaseManagerServerName, "SFWDatabaseManagerServer");

const TUint KServerMajorVersionNumber = 0;
const TUint KServerMinorVersionNumber = 1;
const TUint KServerBuildVersionNumber = 1;

IMPORT_C TInt StartThread(RThread& aServerThread);

enum TDBServerRqst
{
    ERegisterServiceRequest,
    EUnregisterServiceRequest,
    EGetInterfacesRequest,
    EGetServiceNamesRequest,
    EGetServiceNamesSizeRequest,
    EGetInterfacesSizeRequest,
    EInterfaceDefaultRequest,
    EInterfaceDefaultSizeRequest,
    ESetInterfaceDefault,
    ESetInterfaceDefault2,
    ESetChangeNotificationsEnabledRequest,
    ENotifyServiceSignalRequest,
    ECancelNotifyServiceSignalRequest
};

enum TDBServerRqstComplete
{
    ENotifySignalComplete = 1
};

#endif // CLIENTSERVERCOMMON_H_

// End of file
