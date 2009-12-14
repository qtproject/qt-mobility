#include <e32svr.h>
#include "etelmm_etel_test_server.h"
#include "servercommon.h"

void RMobilePhoneStore::Read(TRequestStatus& aReqStatus, TDes8& aEntry) const
	{
	SendReceive( EMPSAsyncRead, TIpcArgs( &aEntry ), aReqStatus );
	}

void RMobilePhoneStore::GetInfo(TRequestStatus& aReqStatus, TDes8& aInfo) const
	{
	SendReceive( EMPSAsyncGetInfo, TIpcArgs( &aInfo ), aReqStatus );
	}

void RMobilePhoneStore::NotifyStoreEvent(TRequestStatus& aReqStatus, TUint32& aEvent, TInt& aIndex) const
	{
	iEvent.Set( (TUint8*)&aEvent, sizeof( aEvent ), sizeof( aEvent ));
	iIndex.Set( (TUint8*)&aIndex, sizeof( aIndex ), sizeof( aIndex ));

	SendReceive( EMPSAsyncNotifyStoreEvent, TIpcArgs( &iEvent, &iIndex ), aReqStatus );
	}

void RMobilePhoneStore::Delete(TRequestStatus& aReqStatus, TInt aIndex) const
	{
	SendReceive( EMPSAsyncDelete, TIpcArgs( aIndex ), aReqStatus );
	}

void RMobilePhoneStore::Write(TRequestStatus& aReqStatus, TDes8& aEntry ) const
	{
	SendReceive( EMPSAsyncWrite, TIpcArgs(&aEntry), aReqStatus );
	}
