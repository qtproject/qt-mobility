#ifndef __SERVERCOMMON_H__
#define __SERVERCOMMON_H__

/** Panic Category */
_LIT(KETelServerPanic, "ETelTestServer");

/** Server name */
_LIT( KETelServerName , "ETel test server" );

_LIT( KETelServerSemaphoreName, "ETel test server semaphore" );

// Message slot definition for the server
const TInt KFirstMsgSlot = 0;
const TInt KSecondMsgSlot = 1;
const TInt KThirdMsgSlot = 2;
const TInt KLastMsgSlot = 3;

enum TETelServerPanic
	{
	ECreateTrapCleanup = 1,
	ECreateServer,
	EBadRequest,
	EReqAlreadyPending,
	EBadDescriptor,
	EBadSubSessionRemove,
	EBadSubSessionType
	};

enum TETelServerRequest
	{
	ENewSubSession = 1,
	ENewSubSessionMPbkStore,
	ECloseSubSession,
	// RTelServer requests
	ETSEnumeratePhones,
	ETSLoadPhoneModule,
	ETSGetPhoneInfo,
	ETSSetExtendedErrorGranularity,
    ENotifySatRefresh,
    ENotifySpecialSatRefresh,
    ECancelNotifySatRefresh,
    EMakeSatRefresh,
	// RMobilePhone requests
	EMPAsyncGetServiceTable,
	EMPAsyncGetServiceTableCancel,
    EMPSyncGetFdnStatus,
	EMPAsyncMobilePhoneNotifyFdnStatusChange,
    EMPAsyncMobilePhoneNotifyFdnStatusChangeCancel,
    EMPSyncGetICCAccessCaps,
    // This is our own RMobilePhone function, not in Symbian's RMobilePhone
    EMPSyncSetFdnStatus,
    // RMmCustomApi requests
    ENewSubSessionMmCustomApi,
	ECPNotifyPndCacheReady,
	ECPNotifyPndCacheReadyCancel,
    ECPGetPndCacheStatus,
    ECPGetPndCacheStatusCancel,
    ECPGet3GPBInfo,
    ECPGet3GPBInfoCancel,
	// RMobilePhoneStore requests
	EMPSAsyncRead,
	EMPSAsyncGetInfo,
	EMPSAsyncNotifyStoreEvent,
	EMPSAsyncDelete,
	EMPSAsyncWrite,
	EMPSAsyncReadCancel,
	EMPSAsyncGetInfoCancel,
	EMPSAsyncNotifyStoreEventCancel,
	EMPSAsyncDeleteCancel,
	EMPSAsyncWriteCancel,
	// RMobilePhoneBookStore requests
	EMPBSRead,
	EMPBSWrite
	};

const TUint KETelServerMajor=0;
const TUint KETelServerMinor=1;
const TUint KETelServerBuild=1;

#endif // __SERVERCOMMON_H__