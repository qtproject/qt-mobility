#include <e32math.h>
#include <e32svr.h>
#include "etelmm_etel_test_server.h"
#include "TestServerDebug.h"

_LIT( KETelServerFileName, "ETelTestServer");

static TInt StartServer();
static TInt CreateServerProcess();

TInt RTelServer::Connect(TInt aMessageSlots)
	{
	TInt error = ::StartServer();

	if(error == KErrNone || error == KErrAlreadyExists)
		{
		error = CreateSession( KETelServerName,
							   Version(),
							   aMessageSlots );
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT CLIENT] RTelServer::Connect:result=%d"),error);
		}

	return error;
	}

TInt RTelServer::EnumeratePhones( TInt& aNoOfPhones ) const
	{
	TPckg<TInt> pckg( aNoOfPhones );
	iNoOfPhones.Set( pckg );
	return SendReceive( ETSEnumeratePhones, TIpcArgs( &iNoOfPhones ) );
	}

TInt RTelServer::LoadPhoneModule(const TDesC& /* aFileName */) const
	{
	return SendReceive( ETSLoadPhoneModule, TIpcArgs() );
	}

TInt RTelServer::GetPhoneInfo(const TInt aIndex, TPhoneInfo& aInfo) const
	{
	TPckg<TPhoneInfo> infoPckg( aInfo );
	iInfo.Set( infoPckg );
	return SendReceive( ETSGetPhoneInfo, TIpcArgs( aIndex, &iInfo ) );
	}

TInt RTelServer::SetExtendedErrorGranularity(const TErrorGranularity /* aGranularity */) const
	{
	return SendReceive( ETSSetExtendedErrorGranularity, TIpcArgs() );
	}

TVersion RTelServer::Version() const
	{
	return TVersion( KETelServerMajor,
					 KETelServerMinor,
					 KETelServerBuild);
	}

void RTelServer::NotifySatRefresh( TRequestStatus& aStatus, TDes16& aFiles,
        TDes8& aType ) const
    {
    SendReceive( ENotifySatRefresh, TIpcArgs( &aFiles, &aType ), aStatus );
    }

void RTelServer::NotifySatRefresh( TRequestStatus& aStatus, TDes16& aFiles,
        TDes8& aType, const TDes16& aInterestedFiles ) const
    {
    SendReceive( ENotifySpecialSatRefresh,
        TIpcArgs( &aFiles, &aType, &aInterestedFiles ), aStatus );
    }

void RTelServer::CancelSatRefreshNotification() const
    {
    SendReceive( ECancelNotifySatRefresh );
    }

void RTelServer::MakeSatRefresh( TDes16& aFiles, TInt aType ) const
    {
    SendReceive( EMakeSatRefresh, TIpcArgs( &aFiles, aType ) );
    }

TInt StartServer()
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING("[ETELT CLIENT] StartServer"));

	TInt result;
	TFindServer findServer( KETelServerName );
	TFullName fullName;

	result = findServer.Next(fullName);
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT CLIENT] find result=%d"), result);

	if(result == KErrNone)
		{
		// Server is already running
		return KErrNone;
		}

	RSemaphore semaphore;
	result = semaphore.CreateGlobal( KETelServerSemaphoreName, 0 );
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT CLIENT] semaphore result=%d"), result);

	if(result != KErrNone)
		{
		return result;
		}

	result = CreateServerProcess();
	if(result != KErrNone)
		{
        semaphore.Close();
		return result;
		}

	semaphore.Wait();
	semaphore.Close();

	return KErrNone;
	}

TInt CreateServerProcess()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT CLIENT] CreateServerProcess"));

	TInt result;
	const TUidType serverUid(KNullUid, KNullUid, KNullUid);

	RProcess server;

	result = server.Create(KETelServerFileName, _L(""), serverUid);
	if(result != KErrNone)
		{
        server.Close();
		return result;
		}

	server.Resume();
	server.Close();

	return KErrNone;
	}
