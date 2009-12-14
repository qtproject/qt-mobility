#include "CETelServer.h"
#include "CETelServerSession.h"
#include "TestServerDebug.h"
#include "CSimStoreEmulator.h"
#include "CSimPhoneEmulator.h"
#include <etelmm.h>

CETelServer* CETelServer::NewLC()
	{
	CETelServer* self = new( ELeave ) CETelServer( EPriorityNormal );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

CETelServer* CETelServer::NewL()
	{
	CETelServer* self = CETelServer::NewLC();
	CleanupStack::Pop();
	return self;
	}

CETelServer::CETelServer(TInt aPriority) :
	CServer2( aPriority ),
	iNumClients( 0 ),
	iSIMPbkEmulators( 10 )
	{
	}

void CETelServer::ConstructL()
	{
	StartL( KETelServerName );
	iContainerIndex = CObjectConIx::NewL();
	iAsyncShutDown = CPeriodic::NewL( EPriorityIdle );
	iSimPhoneEmulator = CSimPhoneEmulator::NewL();
	InitializeSupportedStoresL();
	}

CETelServer::~CETelServer()
	{
    iSatObservers.Close();
	delete iAsyncShutDown;
    delete iContainerIndex;
	iSIMPbkEmulators.ResetAndDestroy();
	delete iSimPhoneEmulator;
	}

CObjectCon* CETelServer::NewContainerL()
	{
	return iContainerIndex->CreateL();
	}

void CETelServer::RemoveObjectContainer( CObjectCon& aContainer )
    {
    iContainerIndex->Remove( &aContainer );
    }

TInt CETelServer::ThreadFunction()
	{
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if(cleanup == 0)
		{
		PanicServer( ECreateTrapCleanup );
		}

	TRAPD(err, ThreadFunctionL());
	if(err != KErrNone)
		{
		PanicServer( ECreateServer );
		}

	delete cleanup;
	cleanup = 0;

	return KErrNone;
	}

void CETelServer::ThreadFunctionL()
	{
	CActiveScheduler *activeScheduler = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(activeScheduler);

	CActiveScheduler::Install(activeScheduler);

	CETelServer::NewLC();

	RSemaphore semaphore;
	User::LeaveIfError( semaphore.OpenGlobal( KETelServerSemaphoreName ) );

	semaphore.Signal();
	semaphore.Close();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); // anon. mathserver, activeSchedule
	}

void CETelServer::IncreaseNumClients()
	{
	iAsyncShutDown->Cancel();
	iNumClients++;
	}

void CETelServer::DecreaseNumClients()
	{
	iNumClients--;
	if(iNumClients == 0)
		{
		for( TInt i( 0 ); i < iSIMPbkEmulators.Count(); i++ )
			{
			iSIMPbkEmulators.At(i)->SaveEntries();
			}
		TCallBack callBack( &CETelServer::IdleDesctruction, this );
		iAsyncShutDown->Start( KETelShutDownAfter, KETelShutDownAfter, callBack );
		}
	}

CSimStoreEmulator& CETelServer::CreateSIMPbkEmulatorL( const TDesC& aStoreName )
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] CreateSIMPbkEmulatorL:%S"), &aStoreName);

	CSimStoreEmulator* emulator = NULL;

	// Check if SIM emulator object already exists.
	for( TInt i( 0 ); i < iSIMPbkEmulators.Count(); i++ )
		{
		if ( iSIMPbkEmulators.At(i)->Name().Compare( aStoreName ) == 0 )
			{
			emulator = iSIMPbkEmulators.At(i);
			break;
			}
		}

	// Emulator didn't exist so create new.
	if ( !emulator )
		{
		emulator = CSimStoreEmulator::NewL( aStoreName );
		CleanupStack::PushL( emulator );
		iSIMPbkEmulators.AppendL( emulator );
		CleanupStack::Pop(); // emulator
		}

	return *emulator;
	}

TArray<CSimStoreEmulator*> CETelServer::SIMPbkEmulators() const
    {
    return iSIMPbkEmulators.Array();
    }

CSimPhoneEmulator&  CETelServer::SimPhoneEmulator() const
    {
    return *iSimPhoneEmulator;
    }

void CETelServer::AddSatRefreshObserverL(
    MSatRefreshEmulationObserver& aObserver )
    {
    if ( iSatObservers.Find( &aObserver ) == KErrNotFound )
        {
        iSatObservers.AppendL( &aObserver );
        }
    }

void CETelServer::RemoveSatRefreshObserver(
    MSatRefreshEmulationObserver& aObserver )
    {
    TInt index = iSatObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iSatObservers.Remove( index );
        }
    }

void CETelServer::MakeSatRefreshL( TDes16& aFiles, TInt aType )
    {
	for( TInt i( 0 ); i < iSIMPbkEmulators.Count(); ++i )
	    {
	    const TSatRefreshFiles& efs =
	        iSIMPbkEmulators.At(i)->ElementaryFiles();
	    for ( TInt j = 0; j < efs.Length(); ++j )
	        {
	        // Refresh only if aFiles contains an EF that a related
	        // to the store.
            if ( aFiles.Locate( efs[j] ) != KErrNotFound )
                {
                iSIMPbkEmulators.At(i)->RefreshL();
                break;
                }
	        }
	    }
    const TInt count = iSatObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iSatObservers[i]->SatRefreshL( aFiles, aType );
        }
    }

TInt CETelServer::RunError( TInt aError )
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] RunError:%d"), aError);
	Message().Complete( aError );
	return KErrNone;
	}

CSession2* CETelServer::NewSessionL(const TVersion& /* aVersion */,
	const RMessage2& /*aMessage*/ ) const
	{
	return CETelServerSession::NewL( *const_cast<CETelServer*>(this) );
	}

void CETelServer::InitializeSupportedStoresL()
    {
    CreateSIMPbkEmulatorL( KETelOwnNumberStore );
    CreateSIMPbkEmulatorL( KETelIccSdnPhoneBook );
    CreateSIMPbkEmulatorL( KETelIccFdnPhoneBook );
    CreateSIMPbkEmulatorL( KETelIccAdnPhoneBook );
    }

void CETelServer::PanicServer( TETelServerPanic aPanic )
	{
	User::Panic( KETelServerPanic, aPanic );
	}

TInt CETelServer::IdleDesctruction( TAny* aSelf )
	{
	CETelServer* self = static_cast<CETelServer*>( aSelf );
	self->StopServer();
	return EFalse;
	}

void CETelServer::StopServer()
	{
	CActiveScheduler::Stop();
	}
