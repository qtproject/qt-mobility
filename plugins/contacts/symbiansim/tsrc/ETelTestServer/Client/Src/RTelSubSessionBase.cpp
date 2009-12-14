#include "etelmm_etel_test_server.h"
#include "RMmCustomAPI_etel_test_server.h"

_LIT( KPanicCategory, "RTelSubSessionBase" );

RTelSubSessionBase::RTelSubSessionBase()
	{
	}

void RTelSubSessionBase::CancelAsyncRequest(TInt aReqToCancel) const
	{
	TInt cancelRequest = MapCancelRequest( aReqToCancel );
	SendReceive( cancelRequest, TIpcArgs() );
	}

TInt RTelSubSessionBase::MapCancelRequest( TInt aRequest ) const
	{
	switch( aRequest )
		{
		case EMobilePhoneStoreGetInfo:
            {
			return EMPSAsyncGetInfoCancel;
            }
		case EMobilePhoneStoreRead:
            {
			return EMPSAsyncReadCancel;
            }
		case EMobilePhoneStoreWrite:
            {
			return EMPSAsyncWriteCancel;
            }
		case EMobilePhoneStoreDelete:
            {
			return EMPSAsyncDeleteCancel;
            }
		case EMobilePhoneStoreNotifyStoreEvent:
            {
			return EMPSAsyncNotifyStoreEventCancel;
            }
		// RMobilePhone
		case EMobilePhoneGetServiceTable:
            {
			return EMPAsyncGetServiceTableCancel;
            }
		case EMobilePhoneNotifyFdnStatusChange:
		    {
		    return EMPAsyncMobilePhoneNotifyFdnStatusChangeCancel;
		    }
        // RMmCustomApi
        case ECustomNotifyPndCacheReadyIPC:
            {
            return ECPNotifyPndCacheReadyCancel;
            }
        case ECustomGetPndCacheStatusIPC:
            {
            return ECPGetPndCacheStatusCancel;
            }
        case EGet3GPBInfoIPC:
            {
            return ECPGet3GPBInfoCancel;
            }
		default:
            {
			User::Panic( KPanicCategory, EBadRequest );
			break;
            }
		}
	return 0;
	}

