/*
* ==============================================================================
*  Name        : CEntryBuffer.cpp
*  Part of     : MM ETel Test Server / ETelTestServer.dll
*  Description :
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
#include    "CEntryBuffer.h"

#include    <mpbutil.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEntryBuffer::CEntryBuffer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEntryBuffer::CEntryBuffer()
    {
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::NewL
// -----------------------------------------------------------------------------
//
CEntryBuffer* CEntryBuffer::NewL()
    {
    CEntryBuffer* self = new( ELeave ) CEntryBuffer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::ConstructL
// -----------------------------------------------------------------------------
//
void CEntryBuffer::ConstructL()
    {
    iETelReadBuffer = new( ELeave ) CPhoneBookBuffer;
    iETelWriteBuffer = new( ELeave ) CPhoneBookBuffer;
    }

// Destructor
CEntryBuffer::~CEntryBuffer()
    {
    delete iETelReadBuffer;
    delete iETelWriteBuffer;
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::CreateServerBufferL
// -----------------------------------------------------------------------------
//
HBufC8* CEntryBuffer::CreateServerBufferLC( TDes8& aClientBuffer,
                                            TInt aETelIndex )
    {
    iWriteBuffer.FillZ();
    iWriteBuffer.Zero();
    iETelWriteBuffer->Set( &iWriteBuffer );
    iETelReadBuffer->Set( &aClientBuffer );
    iETelReadBuffer->StartRead();

    TPtrC16 text;
    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
    TBool readingAnr = EFalse;
    while ( iETelReadBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		text.Set( NULL, 0 );
		switch ( fieldTag )
			{
			case RMobilePhoneBookStore::ETagPBNewEntry:
				{
                iETelWriteBuffer->AddNewEntryTag();
                User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBAdnIndex,
                        (TUint16)aETelIndex ));
				break;
				}
			case RMobilePhoneBookStore::ETagPBText:
				{
				iETelReadBuffer->GetValue( text );
                if ( text.Length() > 0 )
                    {
                    User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBText, text ));
                    }
				break;
				}
			case RMobilePhoneBookStore::ETagPBNumber:
				{
				iETelReadBuffer->GetValue( text );
                if ( text.Length() > 0 )
                    {
                    if ( readingAnr )
                        {
                        readingAnr = EFalse;
                        User::LeaveIfError(
                            iETelWriteBuffer->AddNewNumberTag() );
                        }
                    User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBNumber, text ));
                    }
				break;
				}
            case RMobilePhoneBookStore::ETagPBSecondName:
                {
                iETelReadBuffer->GetValue( text );
                if ( text.Length() > 0 )
                    {
                    User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBSecondName, text ));
                    }
                break;
                }
            case RMobilePhoneBookStore::ETagPBEmailAddress:
                {
                iETelReadBuffer->GetValue( text );
                if ( text.Length() > 0 )
                    {
                    User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBEmailAddress, text ));
                    }
                break;
                }
            case RMobilePhoneBookStore::ETagPBAnrStart:
                {
                readingAnr = ETrue;
                break;
                }
            case RMobilePhoneBookStore::ETagPBGroupName:
                {
                iETelReadBuffer->GetValue( text );
                if ( text.Length() > 0 )
                    {
                    User::LeaveIfError(iETelWriteBuffer->PutTagAndValue(
                        RMobilePhoneBookStore::ETagPBGroupName, text ));
                    }
                break;
                }
            case RMobilePhoneBookStore::ETagPBAdnIndex: // FALLTHROUGH
			default:
				{
				iETelReadBuffer->SkipValue( dataType );
				break;
				}
			}
		}

    return iWriteBuffer.AllocLC();
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::NumOfTextFields
// -----------------------------------------------------------------------------
//
TInt CEntryBuffer::NumOfTextFields( TDes8& aClientBuffer )
    {
    iETelReadBuffer->Set( &aClientBuffer );
    iETelReadBuffer->StartRead();

    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	TBool readingAnr = EFalse;
	TInt count = 0;
    while ( iETelReadBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		if ( dataType == CPhoneBookBuffer::EPhBkTypeDes16 )
		    {
		    ++count;
		    iETelReadBuffer->SkipValue( dataType );
		    }
		else if ( dataType != CPhoneBookBuffer::EPhBkTypeNoData )
		    {
		    iETelReadBuffer->SkipValue( dataType );
		    }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::NumOfFields
// -----------------------------------------------------------------------------
//
TInt CEntryBuffer::NumOfFields( TDes8& aClientBuffer, TUint8 aFieldTag )
    {
    iETelReadBuffer->Set( &aClientBuffer );
    iETelReadBuffer->StartRead();

    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	TBool readingAnr = EFalse;
	TInt count = 0;
    while ( iETelReadBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		if ( dataType != CPhoneBookBuffer::EPhBkTypeNoData )
		    {
		    if ( fieldTag == aFieldTag )
		        {
		        ++count;
		        }
		    iETelReadBuffer->SkipValue( dataType );
		    }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CEntryBuffer::TextFieldAt
// -----------------------------------------------------------------------------
//
TTextField CEntryBuffer::TextFieldAt( TDes8& aClientBuffer, TInt aIndex )
    {
    iETelReadBuffer->Set( &aClientBuffer );
    iETelReadBuffer->StartRead();

    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	TBool readingAnr = EFalse;
	TInt count = 0;
    while ( iETelReadBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		if ( dataType == CPhoneBookBuffer::EPhBkTypeDes16 )
		    {
		    if ( count == aIndex )
		        {
		        TPtrC16 data( KNullDesC );
		        iETelReadBuffer->GetValue( data );
		        return TTextField( data, fieldTag );
		        }
		    ++count;
		    iETelReadBuffer->SkipValue( dataType );
		    }
		else if ( dataType != CPhoneBookBuffer::EPhBkTypeNoData )
		    {
		    iETelReadBuffer->SkipValue( dataType );
		    }
        }
    return TTextField();
    }

//  End of File
