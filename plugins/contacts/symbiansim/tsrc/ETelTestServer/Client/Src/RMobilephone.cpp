#include <e32svr.h>
#include "etelmm_etel_test_server.h"
#include "servercommon.h"
#include "TestServerDebug.h"

RMobilePhone::RMobilePhone()
    :   iTableData( NULL, 0, 0 ),
        iFdnStatus( NULL, 0, 0 ),
        iNotifyFdnStatus( NULL, 0, 0 )
    {
    }

TInt RMobilePhone::Open(RTelServer& aSession,const TDesC& aName)
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT CLIENT] RMobilePhone::Open:name=%S"),&aName);

	SetSessionHandle( aSession );
	return CreateSubSession( aSession, ENewSubSession, TIpcArgs() );
	}

void RMobilePhone::GetServiceTable( TRequestStatus& aReqStatus,
	TMobilePhoneServiceTable aTable, TDes8& aTableData ) const
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT CLIENT] RMobilePhone::GetServiceTable"));

	iTableData.Set( (TUint8*)aTableData.Ptr(),
					aTableData.Length(),
					aTableData.MaxLength() );

	SendReceive( EMPAsyncGetServiceTable, TIpcArgs( aTable, &iTableData ), aReqStatus );
	}

void RMobilePhone::Close()
	{
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT CLIENT] RMobilePhone::Close"));
	CloseSubSession( ECloseSubSession );
	}

TInt RMobilePhone::GetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus) const
    {
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT CLIENT] RMobilePhone::GetFdnStatus"));

    TPckg<TMobilePhoneFdnStatus> pckgBuf( aFdnStatus );
    iFdnStatus.Set( pckgBuf );
    return SendReceive( EMPSyncGetFdnStatus, TIpcArgs( &iFdnStatus ) );
    }

void RMobilePhone::NotifyFdnStatusChange(TRequestStatus& aReqStatus,
    TMobilePhoneFdnStatus& aFdnStatus) const
    {
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT CLIENT] RMobilePhone::NotifyFdnStatusChange"));

    TPckg<TMobilePhoneFdnStatus> pckgBuf( aFdnStatus );
    iNotifyFdnStatus.Set( pckgBuf );
    SendReceive( EMPAsyncMobilePhoneNotifyFdnStatusChange,
        TIpcArgs( &iNotifyFdnStatus ),
        aReqStatus );
    }

TInt RMobilePhone::SetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus)
    {
    TPckg<TMobilePhoneFdnStatus> tmp(aFdnStatus);
    iFdnStatus.Set( tmp );
    return SendReceive( EMPSyncSetFdnStatus, TIpcArgs( &iFdnStatus ));
    }

TInt RMobilePhone::GetIccAccessCaps(TUint32& aCaps) const
    {
    TPckg<TUint32> pckgCaps( aCaps );
    return SendReceive( EMPSyncGetICCAccessCaps, TIpcArgs( &pckgCaps ) );
    }
