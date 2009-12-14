#include <e32svr.h>
#include "etelmm_etel_test_server.h"
#include "TestServerDebug.h"

RMobilePhoneBookStore::RMobilePhoneBookStore() : iWriteIndex( NULL, 0 )
    {
    }

TInt RMobilePhoneBookStore::Open(RMobilePhone& aPhone, const TDesC& aStore)
	{
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    	"[ETELT CLIENT] RMobilePhoneBookStore::Open"));

	RSessionBase* session = &aPhone.SessionHandle();
	SetSessionHandle(*session);
	return CreateSubSession( *session, ENewSubSessionMPbkStore, TIpcArgs( &aStore ) );
	}

void RMobilePhoneBookStore::Read(TRequestStatus& aReqStatus, TInt aIndex, TInt aNumSlots, TDes8& aPBData) const
	{
	SendReceive( EMPBSRead, TIpcArgs( aIndex, aNumSlots, &aPBData ), aReqStatus );
	}

void RMobilePhoneBookStore::Write(TRequestStatus& aReqStatus, const TDesC8& aPBData, TInt& aIndex) const
	{
	TPckg<TInt> pckg( aIndex );
	iWriteIndex.Set( pckg );

	SendReceive( EMPBSWrite, TIpcArgs( &iWriteIndex, &aPBData ), aReqStatus );
	}

void RMobilePhoneBookStore::Close()
	{
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    	"[ETELT CLIENT] RMobilePhoneBookStore::Close"));
	CloseSubSession( ECloseSubSession );
	}
