#include "CETelServerSubSession.h"
#include "CETelServerSession.h"
#include "TestServerDebug.h"
#include "CSimPhoneEmulator.h"
#include <rmmcustomapi.h>

const TUint KGetServiceTableDelay = 10;
const TUint KReadDelay = 20;
const TUint KGetInfoDelay = 10;
const TUint KDeleteDelay = 30;
const TUint KWriteDelay = 60000;


TInt GetServiceTableCB( TAny* aThis )
	{
	return static_cast<CETelServerSubSession*>( aThis )->GetServiceTable();
	}

TInt GetInfoCB( TAny* aThis )
	{
	return static_cast<CETelServerSubSession*>( aThis )->GetInfo();
	}

TInt DeleteCB( TAny* aThis )
	{
	return static_cast<CETelServerSubSession*>( aThis )->Delete();
	}

TInt ReadCB( TAny* aThis )
	{
	return static_cast<CETelServerSubSession*>( aThis )->Read();
	}
	
TInt WriteCB( TAny* aThis )
	{
	return static_cast<CETelServerSubSession*>( aThis )->Write();
	}


//##ModelId=3DB3FD94011D
CETelServerSubSession* CETelServerSubSession::NewL( CETelServerSession& aSession,
                                                    CSimPhoneEmulator& aSimPhoneEmulator,
												    CSimStoreEmulator& aSIMEmulator )
												    
	{
	CETelServerSubSession* self = 
	    new( ELeave ) CETelServerSubSession( aSession, aSimPhoneEmulator, 
	    &aSIMEmulator );
    CleanupClosePushL( *self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CETelServerSubSession* CETelServerSubSession::NewL( CETelServerSession& aSession,
                                                    CSimPhoneEmulator& aSimPhoneEmulator)
												    
	{
	CETelServerSubSession* self = new( ELeave ) CETelServerSubSession( aSession, 
	    aSimPhoneEmulator, NULL );
    CleanupClosePushL( *self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

//##ModelId=3DB3FD9401A0
CETelServerSubSession::CETelServerSubSession( CETelServerSession& aSession, 
											    CSimPhoneEmulator& aSimPhoneEmulator,
						                        CSimStoreEmulator* aSIMEmulator ) :
iSession( &aSession ),
iSimPhoneEmulator( aSimPhoneEmulator ),
iSimStoreEmulator( aSIMEmulator )
	{
	iGetServiceTableCallBack = TCallBack( GetServiceTableCB, this );
	iGetInfoCallBack = TCallBack( GetInfoCB, this );
	iDeleteCallBack = TCallBack( DeleteCB, this );
	iWriteCallBack = TCallBack( WriteCB, this );
	iReadCallBack = TCallBack( ReadCB, this );
	}

//##ModelId=3DB3FD9401AA
void CETelServerSubSession::ConstructL()
	{
	iPeriodicRead = CPeriodic::NewL( CPeriodic::EPriorityStandard );
	if ( !iSimStoreEmulator )
	    {
	    // This is RMmCustomAPI subsession. Start listening to TSY cache
	    // ready event
	    TArray<CSimStoreEmulator*> stores = 
	            iSession->Server().SIMPbkEmulators();
	    for ( TInt i = 0; i < stores.Count(); ++i )
	        {
	        if ( stores[i]->HasTSYCache() && !stores[i]->IsTSYCacheReady() )
	            {
	            stores[i]->AddTSYCacheReadyObserverL( *this );
	            }
	        }
	    }
	}

//##ModelId=3DB3FD94016E
CETelServerSubSession::~CETelServerSubSession()
	{
	TArray<CSimStoreEmulator*> stores = 
            iSession->Server().SIMPbkEmulators();
    for ( TInt i = 0; i < stores.Count(); ++i )
        {
        stores[i]->RemoveTSYCacheReadyObserver( *this );
        }
   
    if ( iSimStoreEmulator )
        {
        iSimStoreEmulator->RemoveNotifyObserver( this );
        }
	    
	delete iPeriodicGetServiceTable;
	delete iPeriodicGetInfo;
	delete iPeriodicDelete;
	delete iPeriodicWrite;
	delete iPeriodicRead;
	}

//##ModelId=3DB3FD940182
TInt CETelServerSubSession::GetServiceTable()
	{
    if ( iWaitingGetServiceTable )
        {
        ETELT_DEBUG_PRINT(
            ETELT_DEBUG_STRING("[ETELT SERVER] Send service table"));
	    iMPGetServiceTableMsg.Complete( KErrNone );
	    delete iPeriodicGetServiceTable;
	    iPeriodicGetServiceTable = NULL;
	    iWaitingGetServiceTable = EFalse;
        }
	return 0;
	}

//##ModelId=3DB3FD940127
void CETelServerSubSession::MPAsyncGetServiceTableL(const RMessage2& aMessage)
	{
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] MPAsyncGetServiceTableL:%d"), 
        iWaitingGetServiceTable );

	if( iWaitingGetServiceTable )
		{
		iSession->PanicClient( EReqAlreadyPending, aMessage );
		}
	else
		{
		iWaitingGetServiceTable = ETrue;
		iMPGetServiceTableMsg = aMessage;
		
		delete iPeriodicGetServiceTable;
		iPeriodicGetServiceTable = NULL;
		iPeriodicGetServiceTable = CPeriodic::NewL( CPeriodic::EPriorityStandard );

		iSimPhoneEmulator.FillServiceTableL( iMPGetServiceTableMsg );

		iPeriodicGetServiceTable->Start( KGetServiceTableDelay, 1, iGetServiceTableCallBack );
		}
	}

void CETelServerSubSession::MPSyncGetFdnStatus(const RMessage2& aMessage)
    {
    RMobilePhone::TMobilePhoneFdnStatus status = iSimPhoneEmulator.FdnStatus();

    TPckg<RMobilePhone::TMobilePhoneFdnStatus> tmp( status );
    aMessage.WriteL( KFirstMsgSlot, tmp );
    aMessage.Complete(KErrNone);
    }

void CETelServerSubSession::MPSyncSetFdnStatus(const RMessage2& aMessage)
    {
    RMobilePhone::TMobilePhoneFdnStatus status = RMobilePhone::EFdnUnknown;
    TPckg<RMobilePhone::TMobilePhoneFdnStatus> tmp(status);
    aMessage.ReadL(KFirstMsgSlot, tmp);
    iSimPhoneEmulator.SetFdnStatusL(status);
    
    aMessage.Complete(KErrNone);
    }
    
void CETelServerSubSession::MPAsyncNotifyFdnStatusChange(const RMessage2& aMessage)
    {
    iSimPhoneEmulator.NotifyFdnStatusChangeL( *this );
    // Save message for canceling
    iMPNotifyFdnStatusChangeMsg = aMessage;
    }
    
void CETelServerSubSession::MPSyncGetICCAccessCapsL(const RMessage2& aMessage)
    {
    TUint32 caps = iSimPhoneEmulator.USimAccessCaps();
    TPckg<TUint32> pckgCaps( caps );
    aMessage.WriteL( KFirstMsgSlot, pckgCaps );
    aMessage.Complete( KErrNone );
    }

//##ModelId=3DB3FD940129
void CETelServerSubSession::MPSAsyncNotifyStoreEventL(const RMessage2& aMessage)
	{
	if( iWaitingNotifyStoreEvent )
		{
		iSession->PanicClient( EReqAlreadyPending, aMessage );
		}
	else
		{
		iWaitingNotifyStoreEvent = ETrue;
		iMPSNotifyStoreEventMsg = aMessage;
		// Add notify observer to emulator
		iSimStoreEmulator->AddNotifyObserverL( this );
		}
	}

//##ModelId=3DB3FD940132
void CETelServerSubSession::MPSAsyncReadL(const RMessage2& aMessage)
	{
	TInt result = iSimStoreEmulator->MPSReadL( aMessage );
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] MPSAsyncReadL complete:%d"), result );
	aMessage.Complete( result );
	}

//##ModelId=3DB3FD94018D
TInt CETelServerSubSession::GetInfo()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] Send Info"));
	iMPSGetInfoMsg.Complete( KErrNone );
	delete iPeriodicGetInfo;
	iPeriodicGetInfo = NULL;
	iWaitingGetInfo = EFalse;		

	return 0;
	}

//##ModelId=3DB3FD94013C
void CETelServerSubSession::MPSAsyncGetInfoL(const RMessage2& aMessage)
	{
	if( iWaitingGetInfo )
		{
		iSession->PanicClient( EReqAlreadyPending, aMessage );
		}
	else
		{
		iWaitingGetInfo = ETrue;
		iMPSGetInfoMsg = aMessage;

		delete iPeriodicGetInfo;
		iPeriodicGetInfo = NULL;
		iPeriodicGetInfo = CPeriodic::NewL( CPeriodic::EPriorityStandard );

		iSimStoreEmulator->FillInfoL( iMPSGetInfoMsg );

		iPeriodicGetInfo->Start( KGetInfoDelay, 1, iGetInfoCallBack );
		}
	}

//##ModelId=3DB3FD940196
TInt CETelServerSubSession::Delete()
	{
	ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] Delete complete"));
	iMPSDeleteMsg.Complete( KErrNone );
	delete iPeriodicDelete;
	iPeriodicDelete = NULL;
	iWaitingDelete = EFalse;

	return 0;
	}

//##ModelId=3DB3FD940145
void CETelServerSubSession::MPSAsyncDeleteL(const RMessage2& aMessage)
	{
	TInt result = iSimStoreEmulator->DeleteL( aMessage );
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] Delete complete:%d"),result);
	aMessage.Complete( result );
	}

TInt CETelServerSubSession::Read()
	{
	iPeriodicRead->Cancel();
	iWaitingRead = EFalse;
	iMPBSAsyncReadMsg.Complete( iMPBSReadResult );
	ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPBSAsyncReadL complete:%d"), 
        iMPBSReadResult );
	return 0;
	}

//##ModelId=3DB3FD940197
TInt CETelServerSubSession::Write()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] Write Complete %d"),
        iMPBSWriteResult);
	iMPBSAsyncWriteMsg.Complete( iMPBSWriteResult );
	delete iPeriodicWrite;
	iPeriodicWrite= NULL;
	iWaitingWrite = EFalse;

	return 0;
	}

//##ModelId=3DB3FD94014F
void CETelServerSubSession::MPSAsyncWriteL(const RMessage2& aMessage)
	{
	if( iWaitingWrite )
		{
		iSession->PanicClient( EReqAlreadyPending, aMessage );
		}
	else
		{
		delete iPeriodicWrite;
        iPeriodicWrite = NULL;
        iPeriodicWrite = CPeriodic::NewL( CPeriodic::EPriorityStandard );
        iWaitingWrite = ETrue;
	    iMPBSWriteResult = iSimStoreEmulator->MPSWriteL( aMessage );
        iMPBSAsyncWriteMsg = aMessage;
        TInt dummyInterval = 1;
        iPeriodicWrite->Start( KWriteDelay,
            dummyInterval, iWriteCallBack );
		}
	}

//##ModelId=3DB3FD940151
void CETelServerSubSession::MPAsyncGetServiceTableCancel()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] Cancel Service Table %d"),
        iWaitingGetServiceTable);

	if( iWaitingGetServiceTable )
		{
		iMPGetServiceTableMsg.Complete( KErrCancel );
		iWaitingGetServiceTable = EFalse;
		}
	}

void CETelServerSubSession::MPAsyncNotifyFdnStatusChangeCancel()
    {
    iSimPhoneEmulator.RemoveFdnStatusChangeL( *this );
    iMPNotifyFdnStatusChangeMsg.Complete(KErrCancel);
    }
    
//##ModelId=3DB3FD940159
void CETelServerSubSession::MPSAsyncNotifyStoreEventCancelL()
	{
	if( iWaitingNotifyStoreEvent )
		{
		iMPSNotifyStoreEventMsg.Complete( KErrCancel );
		iSimStoreEmulator->RemoveNotifyObserver( this );
		iWaitingNotifyStoreEvent = EFalse;
		}
	}

//##ModelId=3DB3FD94015A
void CETelServerSubSession::MPSAsyncReadCancel()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPSAsyncReadCancel %d"), 
        iWaitingRead);

	if( iWaitingRead )
		{
		iWaitingRead = EFalse;
		iPeriodicRead->Cancel();
		iMPBSAsyncReadMsg.Complete( KErrCancel );
		}
	}

//##ModelId=3DB3FD940163
void CETelServerSubSession::MPSAsyncGetInfoCancel()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPSAsyncGetInfoCancel %d"), 
        iWaitingGetInfo);

	if( iWaitingGetInfo )
		{
        if ( iPeriodicGetInfo )
            {
            iPeriodicGetInfo->Cancel();
            }

		iMPSGetInfoMsg.Complete( KErrCancel );
		iWaitingGetInfo = EFalse;
		}
	}

//##ModelId=3DB3FD940164
void CETelServerSubSession::MPSAsyncDeleteCancel()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPSAsyncDeleteCancel %d"), 
        iWaitingDelete);

	if( iWaitingDelete )
		{
		iMPSDeleteMsg.Complete( KErrCancel );
		iWaitingDelete = EFalse;
		}
	}

//##ModelId=3DB3FD94016D
void CETelServerSubSession::MPSAsyncWriteCancel()
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPSAsyncWriteCancel %d"), 
        iWaitingWrite);

	if( iWaitingWrite )
		{
        if ( iPeriodicWrite )
            {
            iPeriodicWrite->Cancel();
            delete iPeriodicWrite;
            iPeriodicWrite = NULL;
            }
		iMPBSAsyncWriteMsg.Complete( KErrCancel );
		iWaitingWrite = EFalse;
		}
	}

void CETelServerSubSession::MPBSAsyncReadL( const RMessage2& aMessage )
	{
    ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPBSAsyncReadL start"));
    
    if (iWaitingRead)
        {
        iSession->PanicClient( EReqAlreadyPending, aMessage );
        }
    else
        {
        iMPBSReadResult = iSimStoreEmulator->ReadL( aMessage );
        iMPBSAsyncReadMsg = aMessage;
        TInt dummyInterval = 1;
        iPeriodicRead->Start( KWriteDelay, 
            dummyInterval, iReadCallBack );
        iWaitingRead = ETrue;
        }
	}

void CETelServerSubSession::MPBSAsyncWriteL( const RMessage2& aMessage )
	{
	ETELT_DEBUG_PRINT(
        ETELT_DEBUG_STRING("[ETELT SERVER] MPBSAsyncWriteL start"));
    if( iWaitingWrite )
		{
		iSession->PanicClient( EReqAlreadyPending, aMessage );
		}
    else
        {
        delete iPeriodicWrite;
        iPeriodicWrite = NULL;
        iPeriodicWrite = CPeriodic::NewL( CPeriodic::EPriorityStandard );
        iWaitingWrite = ETrue;
	    iMPBSWriteResult = iSimStoreEmulator->WriteL( aMessage );
        iMPBSAsyncWriteMsg = aMessage;
        TInt dummyInterval = 1;
        iPeriodicWrite->Start( KWriteDelay, 
            dummyInterval, iWriteCallBack );
        }
	}

//##ModelId=3DB3FD940177
void CETelServerSubSession::CloseSubSession( TUint aHandle,
    const RMessage2& aMessage )
	{
	iSession->DeleteSubSession( aHandle, aMessage );
	}

void CETelServerSubSession::ReceiveNotificationL( TUint32 aEvent, TInt aIndex )
	{
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] ReceiveNotificationL complete,event:%d,index:%d"), 
        aEvent, aIndex );
	
	TPtr8 eventPtr( (TUint8*)&aEvent, sizeof( aEvent ), sizeof( aEvent ) );
	TPtr8 indexPtr( (TUint8*)&aIndex, sizeof( aIndex ), sizeof( aIndex ) );

	iMPSNotifyStoreEventMsg.WriteL( KFirstMsgSlot, eventPtr );
	iMPSNotifyStoreEventMsg.WriteL( KSecondMsgSlot, indexPtr );
	
	iMPSNotifyStoreEventMsg.Complete( KErrNone );
	iWaitingNotifyStoreEvent = EFalse;
	}

void CETelServerSubSession::FdnChangedNotificationL( 
        RMobilePhone::TMobilePhoneFdnStatus aFdnStatus )
    {
    if (iMPNotifyFdnStatusChangeMsg.Handle())
        {
        TPckg<RMobilePhone::TMobilePhoneFdnStatus> tmp( aFdnStatus );
        TInt res = iMPNotifyFdnStatusChangeMsg.Write( KFirstMsgSlot, tmp );
        iMPNotifyFdnStatusChangeMsg.Complete(res);
        }
    }

void CETelServerSubSession::TSYCacheReadyCallback( 
        CSimStoreEmulator& aStoreEmulator )
    {
    if ( iTSYCacheReadyNotification.Handle() != 0 )
        {
        TInt result = iTSYCacheReadyNotification.Write( 
            KFirstMsgSlot, aStoreEmulator.Name() );
        iTSYCacheReadyNotification.Complete( result );
        }
    }
    
void CETelServerSubSession::CustomApiCacheStatusL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLengthL(KSecondMsgSlot);
    HBufC* pbkName = HBufC::NewLC( length );
    TPtr ptr = pbkName->Des();
    aMessage.ReadL( KSecondMsgSlot, ptr );
    
    // Get the store emulator whose cache status are requested
    CSimStoreEmulator& store = iSession->Server().CreateSIMPbkEmulatorL( ptr );
    
    RMmCustomAPI::TPndCacheStatus status = RMmCustomAPI::ECacheNotUsed;
    if ( store.HasTSYCache() )
        {
        status = RMmCustomAPI::ECacheNotReady;
        if ( store.IsTSYCacheReady() )
            {
            status = RMmCustomAPI::ECacheReady;
            }
        }
    
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] CustomApiNotifyL,pbk:%S,status:%d"), 
        &ptr, status );
    CleanupStack::PopAndDestroy(); // pbkName

    TPckg<RMmCustomAPI::TPndCacheStatus> tmp( status );
    aMessage.WriteL( KFirstMsgSlot, tmp );
   
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] CustomApiNotifyL complete"));
    aMessage.Complete(KErrNone);
    }

void CETelServerSubSession::CustomApiCacheStatusCancel(
        const RMessage2& aMessage  )
    {
    // CustomApiCacheStatusL is synchronous so no need request to cancel
    aMessage.Complete( KErrNone );
    }

void CETelServerSubSession::CustomApiNotifyCacheReadyL( 
        const RMessage2& aMessage )
    {
    if ( iTSYCacheReadyNotification.Handle() != 0 )
        {
        User::Leave( KErrAlreadyExists );
        }
    iTSYCacheReadyNotification = aMessage;
    }
    
void CETelServerSubSession::CustomApiNotifyCacheReadyCancel( 
        const RMessage2& aMessage )
    {
    if ( iTSYCacheReadyNotification.Handle() )
        {
        iTSYCacheReadyNotification.Complete( KErrCancel );
        }
    aMessage.Complete( KErrNone );
    }
        
void CETelServerSubSession::Get3GPBInfoL( const RMessage2& aMessage )
    {
    // This is valid only to 3G ADN store
    CSimStoreEmulator& store = 
        iSession->Server().CreateSIMPbkEmulatorL( KETelIccAdnPhoneBook );
    store.FillCustomPhoneBookInfoL( aMessage );
    ETELT_DEBUG_PRINT(
    ETELT_DEBUG_STRING("[ETELT SERVER] Get3GPBInfoL complete"));
    aMessage.Complete( KErrNone );
    }