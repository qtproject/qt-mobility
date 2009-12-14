#include <e32base.h>
#include <e32svr.h>
#include <etelmm.h> // lets use the real MM ETel...

#include "CETelServerSession.h"
#include "CETelServerSubSession.h"
#include "CSimStoreEmulator.h"
#include "TestServerDebug.h"

const TUint KNumberOfPhones = 1;
const TUint KNumberOfLines = 1;
_LIT( KMobilePhoneName, "TESTPHONE" );

/**
* A cleanup class for safely creating a subsession
*/
class TSubSessionCleanup
	{
public:
	TSubSessionCleanup() : iSubSession( NULL ), iIndex( NULL ) {}
	static void Cleanup( TAny* aSelf ) {
        (static_cast<TSubSessionCleanup*>( aSelf ) )->DoCleanup(); }
private:
	void DoCleanup()
		{
		if ( iIndex )
			{
			iIndex->Remove( iHandle );
			}
		else if ( iSubSession )
			{
			iSubSession->Close();
			}
		}
public:
	CObject* iSubSession;
	CObjectIx* iIndex;
	TInt iHandle;
	};

TBool ContainsAtleastOneCommonSIMFile( TDes& aRefreshedSIMFiles,
    const TDesC& aInterestedFiles )
    {
    const TInt fileCount = aRefreshedSIMFiles.Length();
    for ( TInt i = 0; i < fileCount; ++i )
        {
        if ( aInterestedFiles.Locate( aRefreshedSIMFiles[i] ) != KErrNotFound )
            {
            // There is one same file
            return ETrue;
            }
        }
    return EFalse;
    }

//
// CETelServerSession
//
//##ModelId=3DB3FD950170
CETelServerSession* CETelServerSession::NewLC( CETelServer& aServer )
	{
	CETelServerSession* self = new( ELeave ) CETelServerSession( aServer );
	CleanupStack::PushL( self );
	return self;
	}

//##ModelId=3DB3FD95017C
CETelServerSession* CETelServerSession::NewL( CETelServer& aServer )
	{
	CETelServerSession* self = CETelServerSession::NewLC( aServer );
	CleanupStack::Pop();
	return self;
	}

// C++ constructor
//##ModelId=3DB3FD9501AB
CETelServerSession::CETelServerSession( CETelServer& aServer ) :
	iServer( aServer )
	{
	iServer.IncreaseNumClients();
	}

void CETelServerSession::CreateL()
	{
	iContainer = iServer.NewContainerL();
	iSubSessionIx = CObjectIx::NewL();
	}

//##ModelId=3DB3FD95018D
CETelServerSession::~CETelServerSession()
	{
    delete iSubSessionIx;
    iServer.RemoveObjectContainer( *iContainer );
	iServer.DecreaseNumClients();
	delete iInterestedSATFiles;
	}

//##ModelId=3DB3FD9501BF
void CETelServerSession::NewSubSessionL( TInt aSessionType,
	const RMessage2& aMessage )
	{
	CETelServerSubSession* subSession = NULL;

	switch ( aSessionType )
		{
		case ENewSubSession:
			{
			subSession = CETelServerSubSession::NewL( *this, iServer.SimPhoneEmulator() );
			break;
			}
		case ENewSubSessionMPbkStore:
			{
			// Read phonebookname
			const TDesC* clientBuffer = static_cast<const TDesC*>( aMessage.Ptr0() );
			HBufC* serverBuffer = HBufC::NewL( clientBuffer->Length() );
			CleanupStack::PushL( serverBuffer );
			TPtr ptr( serverBuffer->Des() );
			// Read from first message slot
			aMessage.ReadL( 0, ptr );
			CSimStoreEmulator& emulator = iServer.CreateSIMPbkEmulatorL( *serverBuffer );
			CleanupStack::PopAndDestroy(); // serverbuffer

            subSession = CETelServerSubSession::NewL(
                *this, iServer.SimPhoneEmulator(), emulator );
			break;
			}
        case ENewSubSessionMmCustomApi:
            {
            subSession = CETelServerSubSession::NewL(
                *this, iServer.SimPhoneEmulator() );
            break;
            }
		default:
			{
			PanicClient( EBadSubSessionType, aMessage );
			}
		}

	if ( subSession )
		{
        TSubSessionCleanup cleanUp;
        CleanupStack::PushL(
            TCleanupItem( TSubSessionCleanup::Cleanup, &cleanUp ) );

        cleanUp.iSubSession = subSession;
        iContainer->AddL( subSession );
        TInt handle = iSubSessionIx->AddL( subSession );
        cleanUp.iIndex = iSubSessionIx;
        cleanUp.iHandle = handle;
        TPckg<TInt> handlePckg( handle );
	    aMessage.WriteL( KLastMsgSlot, handlePckg );
        CleanupStack::Pop(); // TCleanupItem
		iResourceCount++;
		}
	}

//##ModelId=3DB3FD9501C0
CETelServerSubSession* CETelServerSession::SubSessionFromHandle( TInt aHandle )
	{
	CETelServerSubSession* subSession = (CETelServerSubSession*)iSubSessionIx->At( aHandle );
	if (subSession == NULL)
		{
		// bad handle...
		}
	return subSession;
	}

//##ModelId=3DB3FD9501A2
void CETelServerSession::ServiceL(const RMessage2& aMessage)
	{
    TRAPD(err, DoServiceL(aMessage));
    if (err != KErrNone)
        {
        // if leave occured, we have to signal the message
        // so that the client can continue
        aMessage.Complete(err);
        }
    }

void CETelServerSession::SatRefreshL( TDes16& aFiles, TInt aType )
    {
    TBool notify = ETrue;
    if ( iInterestedSATFiles )
        {
        notify =
            ContainsAtleastOneCommonSIMFile( aFiles, *iInterestedSATFiles );
        }

    if ( iSatMessage.Handle() && notify )
        {
        iSatMessage.WriteL( KFirstMsgSlot, aFiles );
        TPckg<TInt> pckg( aType );
        iSatMessage.WriteL( KSecondMsgSlot, pckg );
        iSatMessage.Complete( KErrNone );
        iServer.RemoveSatRefreshObserver( *this );
        }
    }

void CETelServerSession::DoServiceL(const RMessage2& aMessage)
    {
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CETelServerSession::DoServiceL:function=%d"),
        aMessage.Function() );

	switch( aMessage.Function() )
		{
		case ENewSubSession:
            {
			NewSubSessionL( ENewSubSession, aMessage );
			aMessage.Complete( KErrNone );
			return;
            }
		case ENewSubSessionMPbkStore:
            {
			NewSubSessionL( ENewSubSessionMPbkStore, aMessage );
			aMessage.Complete( KErrNone );
			return;
            }
        case ENewSubSessionMmCustomApi:
            {
            NewSubSessionL(ENewSubSessionMmCustomApi, aMessage );
            aMessage.Complete( KErrNone );
            return;
            }
		// RTelServer requests
		case ETSEnumeratePhones:
            {
			EnumeratePhonesL( aMessage );
			return;
            }
		case ETSLoadPhoneModule:
            {
            aMessage.Complete( KErrNone );
			return;
            }
		case ETSGetPhoneInfo:
            {
			GetPhoneInfoL( aMessage );
			return;
            }
		case ETSSetExtendedErrorGranularity:
            {
            aMessage.Complete( KErrNone );
			return;
            }
        case ENotifySatRefresh:
            {
            HandleNotifySatRefresh( aMessage );
            return;
            }
        case ENotifySpecialSatRefresh:
            {
            HandleSpecialNotifySatRefreshL( aMessage );
            return;
            }
        case ECancelNotifySatRefresh:
            {
            CancelNotifySatRefresh( aMessage );
            return;
            }
        case EMakeSatRefresh:
            {
            MakeSatRefreshL( aMessage );
            return;
            }
		default:
            {
			break;
            }
		}

	CETelServerSubSession* subSession = SubSessionFromHandle( aMessage.Int3() );
	switch( aMessage.Function() )
		{
		case ECloseSubSession:
            {
			subSession->CloseSubSession( aMessage.Int3(), aMessage );
			aMessage.Complete( KErrNone );
			break;
            }
		// RMobilePhone requests
		case EMPAsyncGetServiceTable:
            {
			subSession->MPAsyncGetServiceTableL( aMessage );
			break;
            }
		case EMPAsyncGetServiceTableCancel:
            {
			subSession->MPAsyncGetServiceTableCancel();
			aMessage.Complete( KErrNone );
			break;
            }
        case EMPSyncGetFdnStatus:
            {
            subSession->MPSyncGetFdnStatus( aMessage );
            break;
            }
		case EMPAsyncMobilePhoneNotifyFdnStatusChange:
		    {
		    subSession->MPAsyncNotifyFdnStatusChange( aMessage );
		    break;
		    }
		case EMPAsyncMobilePhoneNotifyFdnStatusChangeCancel:
		    {
		    subSession->MPAsyncNotifyFdnStatusChangeCancel();
		    aMessage.Complete( KErrNone );
		    break;
		    }
        case EMPSyncGetICCAccessCaps:
            {
            subSession->MPSyncGetICCAccessCapsL( aMessage );
            break;
            }
        // Our own RMobilePhone function, not in Symbian RMobilePhone
		case EMPSyncSetFdnStatus:
		    {
		    subSession->MPSyncSetFdnStatus( aMessage );
		    break;
		    }
		// RCtmGsmPhone requests
        // RMMCustomAPI requests
		case ECPNotifyPndCacheReady:
            {
            subSession->CustomApiNotifyCacheReadyL( aMessage );
			break;
            }
		case ECPNotifyPndCacheReadyCancel:
            {
            subSession->CustomApiNotifyCacheReadyCancel( aMessage );
			break;
            }
        case ECPGetPndCacheStatus:
            {
            subSession->CustomApiCacheStatusL( aMessage );
            break;
            }
        case ECPGetPndCacheStatusCancel:
            {
            subSession->CustomApiCacheStatusCancel( aMessage );
            break;
            }
        case ECPGet3GPBInfo:
            {
            subSession->Get3GPBInfoL( aMessage );
            break;
            }
        case ECPGet3GPBInfoCancel:
            {
            aMessage.Complete( KErrCancel );
            break;
            }
		// RMobilePhoneStore requests
		case EMPSAsyncRead:
            {
			subSession->MPSAsyncReadL( aMessage );
			break;
            }
		case EMPSAsyncGetInfo:
            {
			subSession->MPSAsyncGetInfoL( aMessage );
			break;
            }
		case EMPSAsyncNotifyStoreEvent:
            {
			subSession->MPSAsyncNotifyStoreEventL( aMessage );
			break;
            }
		case EMPSAsyncDelete:
            {
			subSession->MPSAsyncDeleteL( aMessage );
			break;
            }
		case EMPSAsyncWrite:
            {
			subSession->MPSAsyncWriteL( aMessage );
			break;
            }
		case EMPSAsyncReadCancel:
            {
			subSession->MPSAsyncReadCancel();
			aMessage.Complete( KErrNone );
            }
			break;
		case EMPSAsyncGetInfoCancel:
            {
			subSession->MPSAsyncGetInfoCancel();
			aMessage.Complete( KErrNone );
			break;
            }
		case EMPSAsyncNotifyStoreEventCancel:
            {
			subSession->MPSAsyncNotifyStoreEventCancelL();
			aMessage.Complete( KErrNone );
			break;
            }
		case EMPSAsyncDeleteCancel:
            {
			subSession->MPSAsyncDeleteCancel();
			aMessage.Complete( KErrNone );
			break;
            }
		case EMPSAsyncWriteCancel:
            {
			subSession->MPSAsyncWriteCancel();
			aMessage.Complete( KErrNone );
			break;
            }
		// RMobilePhoneBookStore requests
		case EMPBSRead:
            {
			subSession->MPBSAsyncReadL( aMessage );
			break;
            }
		case EMPBSWrite:
            {
			subSession->MPBSAsyncWriteL( aMessage );
			break;
            }
		default:
            {
			PanicClient( EBadRequest, aMessage );
			break;
            }
		}
    }

void CETelServerSession::EnumeratePhonesL( const RMessage2& aMessage )
	{
	TUint noOfPhones = KNumberOfPhones;
	TPtr8 noOfPhonesPtr( (TUint8*)&noOfPhones, sizeof( noOfPhones ), sizeof( noOfPhones ) );
	aMessage.WriteL( KFirstMsgSlot, noOfPhonesPtr );
	aMessage.Complete( KErrNone );
	}

void CETelServerSession::GetPhoneInfoL( const RMessage2& aMessage )
	{
	RTelServer::TPhoneInfo info;

	// not used now
	// TInt index = Message().Int0();

	info.iNetworkType = RTelServer::ENetworkTypeMobileDigital;
	info.iName.Copy( KMobilePhoneName );
	info.iNumberOfLines = KNumberOfLines;
	info.iExtensions = 0;

	TPckg<RTelServer::TPhoneInfo> infoPckg( info );
	TDes8& des = infoPckg;
	TPtr8 ptr( (TUint8*)des.Ptr(),
			   des.Length(),
			   des.MaxLength() );

	aMessage.WriteL( KSecondMsgSlot, infoPckg );
	aMessage.Complete( KErrNone );
	}

//##ModelId=3DB3FD950199
void CETelServerSession::DeleteSubSession( TUint aHandle, const RMessage2& aMessage )
	{
	// panic if bad handle
	CETelServerSubSession* subSession = ( CETelServerSubSession* )iSubSessionIx->At( aHandle );
	if ( subSession == NULL )
		PanicClient( EBadSubSessionRemove, aMessage );
	iSubSessionIx->Remove( aHandle );
	// decrement resource count
	iResourceCount--;
	}

void CETelServerSession::HandleNotifySatRefresh( const RMessage2& aMessage )
    {
    if ( iSatMessage.Handle() )
        {
        aMessage.Complete( KErrAlreadyExists );
        }
    else
        {
        iSatMessage = aMessage;
        iServer.AddSatRefreshObserverL( *this );
        }
    }

void CETelServerSession::HandleSpecialNotifySatRefreshL( const RMessage2& aMessage )
    {
    // Get SIM files from client that it's interested in
    TInt bufLength = aMessage.GetDesLengthL( KThirdMsgSlot );
    delete iInterestedSATFiles;
    iInterestedSATFiles = NULL;
    iInterestedSATFiles = HBufC::NewL( bufLength );
    TPtr ptr( iInterestedSATFiles->Des() );
    aMessage.ReadL( KThirdMsgSlot, ptr );

    // There are two versions of NotifySatRefresh and only one of them
    // can be called at a time.
    HandleNotifySatRefresh( aMessage );
    }

void CETelServerSession::CancelNotifySatRefresh( const RMessage2& aMessage )
    {
    if ( iSatMessage.Handle() )
        {
        iSatMessage.Complete( KErrCancel );
        }
    iServer.RemoveSatRefreshObserver( *this );
    aMessage.Complete( KErrNone );
    }

void CETelServerSession::MakeSatRefreshL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLengthL( KFirstMsgSlot );
    HBufC16* files = HBufC16::NewLC( length );
    TPtr16 ptr( files->Des() );
    aMessage.ReadL( KFirstMsgSlot, ptr );
    TInt type = aMessage.Int1();
    aMessage.Complete( KErrNone );

    iServer.MakeSatRefreshL( ptr, type );
    CleanupStack::PopAndDestroy( files );
    }

//##ModelId=3DB3FD95018E
void CETelServerSession::PanicClient( TInt aPanic,
    const RMessage2& aMessage ) const
	{
	aMessage.Panic( KETelServerPanic, aPanic );
	}

