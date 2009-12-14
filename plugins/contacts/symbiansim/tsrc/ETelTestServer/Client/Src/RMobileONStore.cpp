#include <e32svr.h>
#include "etelmm_etel_test_server.h"
#include "servercommon.h"
#include "TestServerDebug.h"

RMobileONStore::RMobileONStore()
	{
	}

TInt RMobileONStore::Open(RMobilePhone& aPhone)
	{
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT CLIENT] RMobileONStore::Open"));

	RSessionBase* session=&aPhone.SessionHandle();
	SetSessionHandle(*session);
	return CreateSubSession( *session, ENewSubSessionMPbkStore,
		TIpcArgs( &KETelOwnNumberStore ) );
	}

void RMobileONStore::Close()
	{
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT CLIENT] RMobileONStore::Close"));
	CloseSubSession( ECloseSubSession );
	}
