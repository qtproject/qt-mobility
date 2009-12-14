/*
* ==============================================================================
*  Name        : CSimStoreEmulator.cpp
*  Part of     : ETelTestServer
*  Description : Emulates the TSY and SIM card
*  Version     :
*
*  Copyright © 2002 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/


// INCLUDE FILES
#include <e32math.h>
#include <etelmmerr.h>
#include "CSimStoreEmulator.h"
#include "SimEmulatorConfig.h"
#include "CEntryBuffer.h"
#include "ServerCommon.h"
#include "TestServerDebug.h"
#include <gsmerror.h>

// CONSTANTS
const TInt KObserversGranularity = 20;
const TInt KEntryArrayGranularity = 20;
const TInt KFirstFreeIndex = -1;

_LIT( KSimPdInternationalPrefix, "+" );


// MODULE DATA STRUCTURES
enum TPanicCode
	{
	EFileReadError = 1,
	EStoreTypeNotSupported,
    ESimEmulatorInvalidPhoneBook,
    ECacheObserverAlreadySet
	};

namespace
    {
    TBool AddIndex( RArray<TInt>& aArray, TInt aIndex )
        {
        // Return ETrue if index was found or added to array
        TBool ret = ETrue;
        
        // Check if given index is found from array
        TInt pos = aArray.Find( aIndex );
        
        if ( pos == KErrNotFound )
            {
            ret = EFalse;
            // Find first free slot
            pos = aArray.Find( KFirstFreeIndex );
            if ( pos != KErrNotFound )
                {
                // Assign the given to the first free slot
                aArray[pos] = aIndex;
                ret = ETrue;
                }
            }
        return ret;
        }

    void RemoveIndex( RArray<TInt>& aArray, TInt aIndex )
        {
        // Find the index from the array and reset the slot
        // if found
        TInt pos = aArray.Find( aIndex );
        if ( pos != KErrNotFound )
            {
            aArray[pos] = KFirstFreeIndex;
            }
        }
    }

CSimStoreEmulator* CSimStoreEmulator::NewL( const TDesC& aStoreName )
	{
	CSimStoreEmulator*  self = new(ELeave) CSimStoreEmulator;
	CleanupStack::PushL( self );
	self->ConstructL( aStoreName );
	CleanupStack::Pop();
	return self;
	}

CSimStoreEmulator::~CSimStoreEmulator()
	{
	iNotifyObservers.Close();
	iCacheReadyObservers.Close();
	delete iTSYCacheReadyTimer;
	delete iPbkName;
	iFs.Close();
	iEntriesWithEmail.Close();
    iEntriesWithAnr.Close();
    iEntriesWithScndName.Close();
	iEntries.ResetAndDestroy();
    delete [] iReservedEntries;
	}

CSimStoreEmulator::CSimStoreEmulator() :
	iEntries( KEntryArrayGranularity ),
	iNotifyObservers ( KObserversGranularity ),
    iEntriesWithEmail( SimEmulator::KMaxEmailEntries ),
    iEntriesWithAnr( SimEmulator::KMaxAnrEntries ),
    iEntriesWithScndName( SimEmulator::KMaxScndNameEntries )
    {
	}

void CSimStoreEmulator::ConstructL( const TDesC& aStoreName )
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::ConstructL:pbk=%S"),&aStoreName);

	iPbkName = aStoreName.AllocL();
	iEntryBuffer = CEntryBuffer::NewL();
    User::LeaveIfError( iFs.Connect() );
    	
    InitializeEmulatorConfigurationL();
    InitializeEmulatorDataStructuresL();
	LoadEntriesL();
	
	// Emulate asynchronous TSY cache building.
	if ( iTSYCacheBuildTime != TTimeIntervalMicroSeconds32(0) )
	    {
	    iTSYCacheReadyTimer = CPeriodic::NewL( CActive::EPriorityIdle );
	    // We need only one callback
	    TTimeIntervalMicroSeconds32 interval = 0;
	    iTSYCacheReadyTimer->Start( iTSYCacheBuildTime, interval, 
	        TCallBack( CSimStoreEmulator::TSYCacheBuildTimerCallback, this ) );
	    }
	}

TInt CSimStoreEmulator::MPSReadL( const RMessage2& aMessage )
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::MPSReadL"));

    // This function implements the RMobilePhoneStore::Read and supports
    // only Own Numbers store.
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg( entry );

	aMessage.ReadL( KFirstMsgSlot, entryPckg );

    // ETel indexes start from 1 but simlator indexes start from 0
	TInt index = entry.iIndex - 1;

    if ( !(SimEmulator::KOwnNumberCaps & RMobilePhoneStore::KCapsReadAccess) )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::MPSReadL:KErrAccessDenied"));
		return KErrAccessDenied;
		}

	// check index validity
	if ( index < 0  || index >= iInfo.iTotalEntries )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::MPSReadL:KErrArgument"));
		return KErrArgument;
		}

	// check if empty
	if ( !iReservedEntries[ index ] )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::MPSReadL:KErrNotFound"));
		return KErrNotFound;
		}

    TPtr8 ptr(iEntries.At(index)->Des());
    TInt result = aMessage.Write( KFirstMsgSlot, ptr );
	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::MPSReadL:end result=%d"),result);
	return result;
	}

TInt CSimStoreEmulator::MPSWriteL( const RMessage2& aMessage )
    {
    // Index is a reference to the client memory
    // This function implements the RMobilePhoneStore::Read and supports
    // only Own Numbers store.
	RMobileONStore::TMobileONEntryV1 entry;
	RMobileONStore::TMobileONEntryV1Pckg entryPckg( entry );
	aMessage.ReadL( KFirstMsgSlot, entryPckg );
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::WriteL:index=%d"), entry.iIndex);

    TInt simIndex = entry.iIndex;
    TInt preCond = CheckWritePreCondition(simIndex);
    if ( preCond != KErrNone )
        {
        return preCond;
        }

    TInt arrayIndex = KErrUnknown;
	// -1 => write to first free location (MM ETEL API)
	if ( simIndex == -1 )
		{
		arrayIndex = GetFirstFreeIndex();
		simIndex = arrayIndex + 1;
		}
	else
		{
		// Our bufferarray starts from 0 index, not 1
		arrayIndex = simIndex - 1;
		}
    entry.iIndex = simIndex;

    HBufC8* cnt = entryPckg.AllocL();
    AddOrReplaceEntryL( cnt, arrayIndex );

    // write contact back so that client gets the sim index
    TInt result = aMessage.Write( KFirstMsgSlot, entryPckg );
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::MPSWriteL:end result=%d"),result);
    return result;
    }

TInt CSimStoreEmulator::ReadL( const RMessage2& aMessage )
	{
	// Our bufferarray starts from 0 index, not 1
	TInt index = aMessage.Int0() - 1;
	TInt numSlots = aMessage.Int1();

    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::ReadL:index=%d:numSlots=%d"),index,numSlots);

	// check rights
	if ( ( iInfo.iCaps & RMobilePhoneStore::KCapsReadAccess ) == 0 )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::ReadL:KErrAccessDenied"));
		return KErrAccessDenied;
		}

	// check index validity
	if ( index < 0  || index >= iInfo.iTotalEntries || numSlots <= 0
                    || index + numSlots > iInfo.iTotalEntries )
		{
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::ReadL:KErrArgument"));
		return KErrArgument;
		}

	// check if slot range has no entries
    TBool hasEntries = EFalse;
	TInt i;
    for ( i = index; i < ( index + numSlots ) && !hasEntries; ++i )
        {
	    if ( iReservedEntries[i] )
		    {
            hasEntries = ETrue;
            }
		}

    if ( !hasEntries )
        {
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::ReadL:KErrNotFound"));
		return KErrNotFound;
        }

    // Get client buffer
	const TDes8* clientBuffer = static_cast<const TDes8*>( aMessage.Ptr2() );
    TInt maxLength = clientBuffer->MaxLength();

    // write entries to client buffer
    TInt offset = 0;
    TInt entryLength = 0;
    TInt result = KErrNone;
    for ( i = index; i < ( index + numSlots ) && (result == KErrNone); ++i )
        {
        if ( iReservedEntries[i] )
            {
            entryLength = iEntries.At(i)->Length();
            // check if buffer has enough space
            if ( ( maxLength - offset) < entryLength )
                {
                // no more entries fit to the given buffer
                break;
                }
                
            TInt result = aMessage.Write( KThirdMsgSlot, *iEntries.At(i), 
                offset );
            if ( result == KErrNone )
                {
                ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
                    "[ETELT SERVER] CSimStoreEmulator::ReadL:slot %d written."),
                        i+1);
                // Entry was written successfully to client's buffer.
                // Update offset
                offset = offset + entryLength;

                TPtr8 entryBuf( iEntries.At( i )->Des() );
                if (  iEntryBuffer->NumOfFields( 
                        entryBuf, RMobilePhoneBookStore::ETagPBEmailAddress ) > 0 )
                    {
                    AddIndex( iEntriesWithEmail, i );
                    }
                // If there are more than 1 number the entry contains additional number(s)
                if (  iEntryBuffer->NumOfFields( 
                        entryBuf, RMobilePhoneBookStore::ETagPBNumber ) > 1 )
                    {
                    AddIndex( iEntriesWithAnr, i );
                    }    
                }            
            }
        }
        
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::ReadL:end:result=%d"), result);
	return result;
	}

TInt CSimStoreEmulator::WriteL( const RMessage2& aMessage )
    {
    // Index is a reference to the client memory
    TInt simIndex = KErrUnknown;
    TPckg<TInt> indexPckg( simIndex );
    aMessage.ReadL( KFirstMsgSlot, indexPckg );

    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::WriteL:simIndex=%d"), simIndex);

    TInt preCond = CheckWritePreCondition(simIndex);
    if ( preCond != KErrNone )
        {
        return preCond;
        }

    TInt arrayIndex = KErrUnknown;
	// -1 => write to first free location (MM ETEL API)
	if ( simIndex == -1 )
		{
		arrayIndex = GetFirstFreeIndex();
		simIndex = arrayIndex + 1;
		}
	else
		{
		// Our bufferarray starts from 0 index, not 1
		arrayIndex = simIndex - 1;
		}

    // clientBuffer needs to be read as TDes8
	TDes8* clientBuffer = const_cast<TDes8*>
        ( static_cast< const TDes8* >( aMessage.Ptr1() ) );

    // Check that is "Email file" full. In USIM this means EF(EMAIL).
    if ( iEntryBuffer->NumOfFields( 
            *clientBuffer, RMobilePhoneBookStore::ETagPBEmailAddress ) > 0 )
        {
        if ( !AddIndex( iEntriesWithEmail, arrayIndex ) )
            {
            ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL: Email full"));
            HBufC8* serverBuf = iEntryBuffer->CreateServerBufferLC( *clientBuffer, simIndex );
            CleanupStack::Pop(serverBuf);

            AddOrReplaceEntryL( serverBuf, arrayIndex );

        	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL:end KErrNone"));
            // Write the index to the client memory, client gives a TInt& for the index
            TPckg<TInt> etelIndexPckg( simIndex );
            aMessage.WriteL( KFirstMsgSlot, etelIndexPckg );
            return KErrGsmSimServEmailFull;
            }
        }

    // If there are more than 1 number the entry contains additional number(s).
    // Check if "Additiona number file" is full. In USIM this means EF(ANR)
    if (  iEntryBuffer->NumOfFields( 
            *clientBuffer, RMobilePhoneBookStore::ETagPBNumber ) > 1 )
        {
        if ( !AddIndex( iEntriesWithAnr, arrayIndex ) )
            {
            ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL: ANR full"));
            HBufC8* serverBuf = iEntryBuffer->CreateServerBufferLC( *clientBuffer, simIndex );
            CleanupStack::Pop(serverBuf);

            AddOrReplaceEntryL( serverBuf, arrayIndex );

        	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL:end KErrNone"));
            // Write the index to the client memory, client gives a TInt& for the index
            TPckg<TInt> etelIndexPckg( simIndex );
            aMessage.WriteL( KFirstMsgSlot, etelIndexPckg );
            return KErrGsmSimServAnrFull;
            }
        }

    // Check that is the "second name file" full. In USIM this means EF(SNE). 
    if (  iEntryBuffer->NumOfFields( 
            *clientBuffer, RMobilePhoneBookStore::ETagPBSecondName ) > 0 )
        {
        if ( !AddIndex( iEntriesWithScndName, arrayIndex ) )
            {
            ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL: SNE full"));
            HBufC8* serverBuf = iEntryBuffer->CreateServerBufferLC( *clientBuffer, simIndex );
            CleanupStack::Pop(serverBuf);

            AddOrReplaceEntryL( serverBuf, arrayIndex );

        	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
            "[ETELT SERVER] CSimStoreEmulator::WriteL:end KErrNone"));
            // Write the index to the client memory, client gives a TInt& for the index
            TPckg<TInt> etelIndexPckg( simIndex );
            aMessage.WriteL( KFirstMsgSlot, etelIndexPckg );
            return KErrGsmSimServSneFull;
            }
        }
    
    // Find the GSM number that should be always the first number in the buffer
    const TInt fieldCount = iEntryBuffer->NumOfTextFields( *clientBuffer );
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        TTextField field = iEntryBuffer->TextFieldAt( *clientBuffer, i );
        if ( field.iFieldType == RMobilePhoneBookStore::ETagPBNumber )
            {
            TInt maxNumberLength = SimEmulator::KMaxADNNumberLength;
            if ( field.iTextData.Locate( KSimPdInternationalPrefix()[0] ) == 0 )
                {
                // '+' is not counted to max number length
                ++maxNumberLength;
                }
            if ( field.iTextData.Length() > maxNumberLength )
                {
                return KErrGsm0707DialStringTooLong;
                }
            }
        }
    
    HBufC8* serverBuf = iEntryBuffer->CreateServerBufferLC( *clientBuffer, simIndex );
    CleanupStack::Pop(serverBuf);

    AddOrReplaceEntryL( serverBuf, arrayIndex );

	ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::WriteL:end KErrNone"));
    // Write the index to the client memory, client gives a TInt& for the index
    TPckg<TInt> etelIndexPckg( simIndex );
    aMessage.WriteL( KFirstMsgSlot, etelIndexPckg );
	return KErrNone;
	}

TInt CSimStoreEmulator::DeleteL( const RMessage2& aMessage )
	{
	TInt index = aMessage.Int0() - 1;
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::DeleteL:index=%d"),index);

	// check rights
	if ( ( iInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess ) == 0 )
		{
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::DeleteL:KCapsWriteAccess"));
		return KErrAccessDenied;
		}

	// check index validity
	if ( index < 0  || index >= iInfo.iTotalEntries )
		{
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::DeleteL:KErrArgument"));
		return KErrArgument;
		}

	// check if empty
	if ( !iReservedEntries[index] )
		{
        ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::DeleteL:KErrNotFound"));
		return KErrNotFound;
		}

	delete iEntries[index];
	iEntries[index] = NULL;
	iReservedEntries[index] = EFalse;
	--iInfo.iUsedEntries;
    RemoveIndex( iEntriesWithEmail, index );
    RemoveIndex( iEntriesWithAnr, index );
    RemoveIndex( iEntriesWithScndName, index );

	// add one to index because on SIM they are indexed from 1
	NotifyObserversL( RMobilePhoneStore::KStoreEntryDeleted, index + 1 );
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::DeleteL:KErrNone"));
	return KErrNone;
	}

void CSimStoreEmulator::FillInfoL( const RMessage2& aMessage )
	{
	// First check what kind of store info is requested
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 info;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg infoPckg( info );

	aMessage.ReadL( KFirstMsgSlot, infoPckg );
	switch( info.iType )
		{
		case RMobilePhoneStore::EPhoneStoreTypeUnknown:
			Panic( EStoreTypeNotSupported );
			break;
		case RMobilePhoneStore::EShortMessageStore:
			Panic( EStoreTypeNotSupported );
			break;
		case RMobilePhoneStore::ENamStore:
			Panic( EStoreTypeNotSupported );
			break;
		case RMobilePhoneStore::EPhoneBookStore:
			FillPhoneBookStoreInfoL( aMessage );
			break;
		case RMobilePhoneStore::EEmergencyNumberStore:
			Panic( EStoreTypeNotSupported );
			break;
		case RMobilePhoneStore::EOwnNumberStore:
			FillOwnNumberStoreInfoL( aMessage );
			break;
		default:
			Panic( EStoreTypeNotSupported );
			break;
		}
	}

void CSimStoreEmulator::FillCustomPhoneBookInfoL( const RMessage2& aMessage )
    {
    TPckg<RMmCustomAPI::T3GPBInfo> infoPckg( i3GPInfo );
    aMessage.WriteL( KFirstMsgSlot, infoPckg );
    }

void CSimStoreEmulator::FillPhoneBookStoreInfoL( const RMessage2& aMessage )
	{
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg infoPckg( iInfo );
	aMessage.WriteL( KFirstMsgSlot, infoPckg );
	}

void CSimStoreEmulator::FillOwnNumberStoreInfoL( const RMessage2& aMessage )
	{
    RMobileONStore::TMobileONStoreInfoV1 info;
	RMobileONStore::TMobileONStoreInfoV1Pckg infoPckg( info );

    info.iName.Copy( iInfo.iName );
	info.iType = iInfo.iType;
	info.iTotalEntries = iInfo.iTotalEntries;
	info.iUsedEntries = iInfo.iUsedEntries;
	info.iCaps = iInfo.iCaps;
	info.iNumberLen = iInfo.iMaxNumLength;
	info.iTextLen = iInfo.iMaxTextLength;

	aMessage.WriteL( KFirstMsgSlot, infoPckg );
	}

void CSimStoreEmulator::AddNotifyObserverL( MStoreNotifyObserver* aObserver )
	{
	// sort by handle
	iNotifyObservers.AppendL( aObserver );
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::AddNotifyObserverL:pbk=%S"), iPbkName);
	}

void CSimStoreEmulator::RemoveNotifyObserver( MStoreNotifyObserver* aObserver )
	{
    ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
    "[ETELT SERVER] CSimStoreEmulator::RemoveNotifyObserverL:pbk=%S"), iPbkName);
    
    TInt index = iNotifyObservers.Find( aObserver );
    if ( index != KErrNotFound )
        {
        iNotifyObservers.Remove( index );
        }
	}

const TDesC& CSimStoreEmulator::Name() const
	{
	return *iPbkName;
	}

TBool CSimStoreEmulator::HasTSYCache() const
    {
    return iTSYCacheBuildTime != TTimeIntervalMicroSeconds32(0);
    }

TBool CSimStoreEmulator::IsTSYCacheReady() const
    {
    if ( HasTSYCache() )
        {
        // Cache is ready if the timer doesn't exist anymore.
        return !iTSYCacheReadyTimer;
        }
    return EFalse;
    }
    
void CSimStoreEmulator::AddTSYCacheReadyObserverL( 
        MTSYCacheReadyObserver& aObserver )
    {
    // Only one observer supported, add multiple support if needed.
    iCacheReadyObservers.AppendL( &aObserver );
    }
    
void CSimStoreEmulator::RemoveTSYCacheReadyObserver(
        MTSYCacheReadyObserver& aObserver )
    {
    TInt index = iCacheReadyObservers.Find( &aObserver );
    if ( index != KErrNotFound  )
        {
        iCacheReadyObservers.Remove( index );
        }
    }

const TSatRefreshFiles& CSimStoreEmulator::ElementaryFiles() const
    {
    return iSatFilesBuf;
    }

void CSimStoreEmulator::RefreshL()
    {
    InitializeEmulatorConfigurationL();
    InitializeEmulatorDataStructuresL();
    // Read again the contacts that existed in server start-up
	LoadEntriesL();
    }

void CSimStoreEmulator::NotifyObserversL( TUint32 aEvent, TInt aIndex )
	{
	TInt count( iNotifyObservers.Count() );
	for( TInt i = 0; i < count; ++i )
		{
		iNotifyObservers[i]->ReceiveNotificationL( aEvent, aIndex );
		}
	// observers have to register again for new events.
	iNotifyObservers.Reset();
	}

void CSimStoreEmulator::SaveEntries()
	{
	RFileWriteStream writeStream;

	TRAPD( err, SaveEntriesL( writeStream ) );
	if ( err != KErrNone )
		{
		writeStream.Close();
		TFileName filename;
        filename.Format( SimEmulator::KDatabaseName, iPbkName );
		iFs.Delete( filename );
		}
	}

void CSimStoreEmulator::SaveEntriesL( RFileWriteStream& aWriteStream )
	{
	TFileName filename;
    filename.Format( SimEmulator::KDatabaseName, iPbkName );

	if ( aWriteStream.Replace( iFs, filename, EFileStream ) == KErrNone )
		{
		// Write the amount of the slots in beginning of the file.
		aWriteStream.WriteInt32L( iInfo.iTotalEntries );

		HBufC8* buffer;
		for ( TInt i( 0 ); i < iInfo.iTotalEntries; i++ )
			{
			buffer = iEntries.At( i );
			// If slot is reserved => write the length and and the buffer.
			if ( buffer )
				{
				aWriteStream.WriteInt32L( buffer->Length() );
				aWriteStream.WriteL( *buffer );
				}
			// If slot is empty => save only 0 length
			else
				{
				aWriteStream.WriteInt32L( 0 );
				}
			}
		aWriteStream.Close();
		}
	}

void CSimStoreEmulator::LoadEntriesL()
	{
	RFileReadStream readStream;
	readStream.PushL();

	TFileName filename;
    filename.Format( SimEmulator::KDatabaseName, iPbkName );
	//TInt result( readStream.Open( iFs, filename, EFileRead ) );
	TInt result( readStream.Open( iFs, filename, EFileStream ) );
	if ( result == KErrNone )
		{
		// possible trap
		TInt numOfEntries( readStream.ReadInt32L() );

		TInt length( 0 );
		HBufC8* buffer;

		// Try to read all the slots
		for ( TInt i( 0 ); i < iInfo.iTotalEntries; i++ )
			{
			if ( i < numOfEntries )
				{
				length = readStream.ReadInt32L();
				}
			else // there was less slots in file than should be
				{
				break;
				}

			if ( length > 0 )
				{
				buffer = HBufC8::NewLC( length );
				TPtr8 ptr( buffer->Des() );
				readStream.ReadL( ptr, length );
				iEntries[i] = buffer;
				CleanupStack::Pop(); // buffer
				iReservedEntries[i] = ETrue;
                ++iInfo.iUsedEntries;
                TPtr8 entryBuf( iEntries.At( i )->Des() );
                if ( iEntryBuffer->NumOfFields( 
                    entryBuf, RMobilePhoneBookStore::ETagPBEmailAddress ) > 0 )
                    {
                    AddIndex( iEntriesWithEmail, i );
                    }
                // If there are more than 1 number the entry contains additional number(s)
                if (  iEntryBuffer->NumOfFields( 
                        entryBuf, RMobilePhoneBookStore::ETagPBNumber ) > 1 )
                    {
                    AddIndex( iEntriesWithAnr, i );
                    }
				}
			}
		readStream.Close();
		}
	readStream.Pop();
	}

void CSimStoreEmulator::InitializeEmulatorConfigurationL()
    {
    // Set used entries to zero.
    iInfo.iUsedEntries = 0;
    // Reset SAT files
    iSatFilesBuf.FillZ();
    iSatFilesBuf.Zero();
    
    // ADN
    if ( iPbkName->Compare( KETelIccAdnPhoneBook ) == 0 )
        {
        iTSYCacheBuildTime = SimEmulator::KTSYCacheBuildingTimeForADN;
        iInfo.iName.Copy( KETelIccAdnPhoneBook );
	    iInfo.iType = RMobilePhoneStore::EPhoneBookStore;
        iInfo.iTotalEntries = SimEmulator::KMaxADNEntries;
        if ( SimEmulator::KUSimAccessSupported )
            {
            iInfo.iCaps = SimEmulator::KUSimADNCaps;    
            }
        else
            {
            iInfo.iCaps = SimEmulator::KSimADNCaps;
            }
        
        iInfo.iMaxNumLength = SimEmulator::KMaxADNNumberLength;
        iInfo.iMaxTextLength = SimEmulator::KMaxADNTextLength;

        i3GPInfo.iMaxLenEmail = SimEmulator::KMaxEmailLength;
        i3GPInfo.iMaxLenSne = SimEmulator::KMaxScndNameLength;
        i3GPInfo.iMaxLenAnr = SimEmulator::KMaxAnrLength;
        i3GPInfo.iMaxLenGroupName = SimEmulator::KMaxGroupNameLength;
        i3GPInfo.iMaxNumEmail = SimEmulator::KMaxEMailFields;
        i3GPInfo.iMaxNumSne = SimEmulator::KMaxScndNameFields;
        i3GPInfo.iMaxNumAnr = SimEmulator::KMaxAnrFields;
        i3GPInfo.iMaxNumGroupName = SimEmulator::KMaxGroupNameFields;

        // Initialize EFs that are at least used by this store
        iSatFilesBuf.Append( KAdnEf );
        iSatFilesBuf.Append( KExt1Ef );
        }
    // SDN
    else if ( iPbkName->Compare( KETelIccSdnPhoneBook ) == 0 )
        {
        iTSYCacheBuildTime = SimEmulator::KTSYCacheBuildingTimeForSDN;
        iInfo.iName.Copy( KETelIccSdnPhoneBook );
	    iInfo.iType = RMobilePhoneStore::EPhoneBookStore;
        iInfo.iTotalEntries = SimEmulator::KMaxSDNEntries;
        iInfo.iCaps = SimEmulator::KSDNCaps;
        iInfo.iMaxNumLength = SimEmulator::KMaxSDNNumberLength;
        iInfo.iMaxTextLength = SimEmulator::KMaxSDNTextLength;
        
        iSatFilesBuf.Append( KSdnEf );
        iSatFilesBuf.Append( KExt3Ef );
        }
    // FDN
    else if ( iPbkName->Compare( KETelIccFdnPhoneBook ) == 0 )
        {
        iTSYCacheBuildTime = SimEmulator::KTSYCacheBuildingTimeForFDN;
        iInfo.iName.Copy( KETelIccFdnPhoneBook );
	    iInfo.iType = RMobilePhoneStore::EPhoneBookStore;
        iInfo.iTotalEntries = SimEmulator::KMaxFDNEntries;
        iInfo.iCaps = SimEmulator::KFDNCaps;
        iInfo.iMaxNumLength = SimEmulator::KMaxFDNNumberLength;
        iInfo.iMaxTextLength = SimEmulator::KMaxFDNTextLength;
        
        iSatFilesBuf.Append( KFdnEf );
        iSatFilesBuf.Append( KExt2Ef );
        }
    // MSISDN
    else if ( iPbkName->Compare( KETelOwnNumberStore ) == 0 )
        {
        iTSYCacheBuildTime = SimEmulator::KTSYCacheBuildingTimeForMSISDN;
        iInfo.iName.Copy( KETelOwnNumberStore );
	    iInfo.iType = RMobilePhoneStore::EOwnNumberStore;
        iInfo.iTotalEntries = SimEmulator::KMaxOwnNumberEntries;
        iInfo.iCaps = SimEmulator::KOwnNumberCaps;
        iInfo.iMaxNumLength = SimEmulator::KMaxOwnNumberNumberLength;
        iInfo.iMaxTextLength = SimEmulator::KMaxOwnNumberTextLength;
        
        iSatFilesBuf.Append( KMsisdnEf );
        iSatFilesBuf.Append( KExt5Ef );
        }
    else
        {
        Panic( ESimEmulatorInvalidPhoneBook );
        }
    }

void CSimStoreEmulator::InitializeEmulatorDataStructuresL()
    {
    delete iReservedEntries;
    iReservedEntries = NULL;
    iReservedEntries = new( ELeave ) TBool[iInfo.iTotalEntries];
    
    for ( TInt i = 0; i < iInfo.iTotalEntries; ++i )
        {
        iReservedEntries[i] = EFalse;
        }
    
    TInt i( 0 );
    iEntriesWithEmail.Reset();
    for ( i = 0; i < SimEmulator::KMaxEmailEntries; ++i )
        {
        iEntriesWithEmail.AppendL( KFirstFreeIndex );
        }
    
    iEntriesWithAnr.Reset();
    for ( i = 0; i < SimEmulator::KMaxAnrEntries; ++i )
        {
        iEntriesWithAnr.AppendL( KFirstFreeIndex );
        }
    
    iEntriesWithScndName.Reset();
    for ( i = 0; i < SimEmulator::KMaxScndNameEntries; ++i )
        {
        iEntriesWithScndName.AppendL( KFirstFreeIndex );
        }
    
    iEntries.Reset();
	for ( i = 0; i < iInfo.iTotalEntries; ++i )
		{
		iEntries.AppendL( NULL );
		}
    }
    
TInt CSimStoreEmulator::GetFirstFreeIndex()
    {
    for ( TInt i = 0; i < iInfo.iTotalEntries; ++i )
    	{
    	if ( !iReservedEntries[i] )
    		{
    		return i;
    		}
    	}
    return -1;
    }

TInt CSimStoreEmulator::CheckWritePreCondition( TInt aSimIndex )
    {
    TInt result = KErrNone;
    // check index validity
	if ( aSimIndex < -1  || aSimIndex == 0 || aSimIndex > iInfo.iTotalEntries )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::WriteL:KErrArgument"));
		result = KErrArgument;
		}
	else if ( aSimIndex == -1 && GetFirstFreeIndex() == -1)
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::WriteL:KErrMMEtelMaxReached"));
		result = KErrMMEtelMaxReached;
		}
	// check rights
	else if ( ( iInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess ) == 0 )
		{
		ETELT_DEBUG_PRINT(ETELT_DEBUG_STRING(
        "[ETELT SERVER] CSimStoreEmulator::WriteL:KErrAccessDenied"));
		result = KErrAccessDenied;
		}
    return result;
    }

void CSimStoreEmulator::AddOrReplaceEntryL( HBufC8* aEntry, TInt aArrayIndex )
    {
    CleanupStack::PushL(aEntry);
    TUint32 events = 0;
	if ( iReservedEntries[ aArrayIndex ] )
		{
		iEntries.Delete( aArrayIndex );
		iEntries.InsertL( aArrayIndex, aEntry);
		events = RMobilePhoneStore::KStoreEntryChanged;
		}
	else
		{
		iEntries[ aArrayIndex ] = aEntry;
        events = RMobilePhoneStore::KStoreEntryAdded;
        ++iInfo.iUsedEntries;
		}
    CleanupStack::Pop(aEntry);
	iReservedEntries[ aArrayIndex ] = ETrue;

	if ( iInfo.iUsedEntries == iInfo.iTotalEntries )
		{
		events = events | RMobilePhoneStore::KStoreFull;
		}

	if ( iInfo.iCaps & RMobilePhoneBookStore::KCapsNotifyEvent )
	    {
	    // add one to index because on SIM they are indexed from 1
	    TInt etelIndex = aArrayIndex + 1;
	    NotifyObserversL( events, etelIndex );
	    }
    }

TInt CSimStoreEmulator::TSYCacheBuildTimerCallback( TAny* aThis )
    {
    CSimStoreEmulator* emulator = static_cast<CSimStoreEmulator*>( aThis );
    const TInt count = emulator->iCacheReadyObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        emulator->iCacheReadyObservers[i]->TSYCacheReadyCallback( *emulator );
        }
    delete emulator->iTSYCacheReadyTimer;
    emulator->iTSYCacheReadyTimer = NULL;
    return EFalse;
    }
    
void CSimStoreEmulator::Panic( TInt aReason )
	{
	_LIT( KPanicText, "CSimStoreEmulator" );
	User::Panic( KPanicText, aReason);
	}
