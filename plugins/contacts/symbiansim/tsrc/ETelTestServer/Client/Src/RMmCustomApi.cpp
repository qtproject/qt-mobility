#include <e32svr.h>
#include "RMmCustomApi_etel_test_server.h"
#include "servercommon.h"
#include "TestServerDebug.h"

RMmCustomAPI::RMmCustomAPI()
: iPbkName1(NULL,NULL),
  iPbkName2(NULL,NULL),
  iCacheStatus(NULL,NULL),
  i3GPBInfo(NULL,NULL)
    {
    }

TInt RMmCustomAPI::Open(RMobilePhone& aPhone)
    {
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    	"[ETELT CLIENT] RMmCustomAPI::Open"));

	RSessionBase* session=&aPhone.SessionHandle();
	SetSessionHandle(*session);
	return CreateSubSession( *session, ENewSubSessionMmCustomApi, TIpcArgs() );
    }

void RMmCustomAPI::Close()
    {
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT CLIENT] RMmCustomAPI::Close"));
	CloseSubSession( ECloseSubSession );
    }

void RMmCustomAPI::NotifyPndCacheReady(TRequestStatus& aStatus, TName& aPndName)
    {
    SendReceive(ECPNotifyPndCacheReady, TIpcArgs( &aPndName ), aStatus);
    }

void RMmCustomAPI::GetPndCacheStatus( TRequestStatus& aStatus,
            RMmCustomAPI::TPndCacheStatus& aPndStatus,
            const TName& aPndName ) const
    {
    TPckg<RMmCustomAPI::TPndCacheStatus> pndStatus( aPndStatus );
    iCacheStatus.Set( pndStatus );
    SendReceive(ECPGetPndCacheStatus, TIpcArgs( &iCacheStatus, &aPndName ), aStatus);
    }

void RMmCustomAPI::Get3GPBInfo( TRequestStatus& aStatus,
            T3GPBInfo& aInfo )
    {
    TPckg<T3GPBInfo> info( aInfo );
    i3GPBInfo.Set( info );
    SendReceive( ECPGet3GPBInfo, TIpcArgs( &i3GPBInfo ), aStatus );
    }
