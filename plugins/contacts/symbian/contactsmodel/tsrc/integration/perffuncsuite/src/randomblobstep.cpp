/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file 
 @publishedAll
 @released
*/
#include <cntfldst.h>
#include "randomblobstep.h"
#include "performancefunctionalitydefs.h"

const TInt KKByte = 1024;

_LIT(KRun1,"BlobTests");
_LIT(KRun2,"MaxBlob");

_LIT(KTest1, "Blobs tests");
_LIT(KTest2, "Max Blob size test");

#define KNumberOfContacts 10

CRandomBlobStep::CRandomBlobStep() : CPerformanceFunctionalityBase( KNumberOfContacts )
	{
	SetTestStepName(KRandomBlobStep);
	TTime time;
	time.UniversalTime();
	iSeed = time.Int64();
	}

TVerdict CRandomBlobStep::doTestStepL()
	{
	__UHEAP_MARK;
	InitializeL();
	_LIT(KDoStepPrint,"CRandomBlobStep::doTestStepL()");
	INFO_PRINTF1(KDoStepPrint);  //Block start 
	
	const TDesC &run = ConfigSection();
	
	if( run == KRun1 )
		{
		INFO_PRINTF1(KTest1);
		BlobTestL();
		}
	else if( run == KRun2 )
		{
		INFO_PRINTF1(KTest2);
		MaxBlobTestL();
		}
	else
		{
		MissingTestPanic();
		}
	Cleanup();
	__UHEAP_MARKEND;

	return TestStepResult();
	}

/**
creates a blob with the specified size and fills it with random values
*/
HBufC8 *CRandomBlobStep::RandomBlobLC(const TInt aSize)
	{
	
	HBufC8 *buf = HBufC8::NewLC( aSize );
	TPtr8 tptr8 = buf->Des();
		
	for( TInt i = 0; i < aSize; ++i)
		{
		tptr8.Append( TChar( static_cast<TUint>( Math::Rand(iSeed) % 256 ) ) );
		}

	return buf;
	}

/**
sets the value of all blobs in a contact (aCid) with aBuf
*/
void CRandomBlobStep::SetAllBlobsL(const TContactItemId aCid, const HBufC8 &aBuf)
	{
	OpenL( aCid );
	TInt length = iFields->Count();
	for( TInt i = 0; i < length; ++i )
		{
		if( (*iFields)[i].StorageType() == KStorageTypeStore )
			{
			(*iFields)[i].StoreStorage()->SetThingL(aBuf);
			}
		}
	CommitL( EFalse );
	}

/**
checks that all blobs in aCid match aBuf
*/
TBool CRandomBlobStep::CheckAllBlobsL(const TContactItemId aCid, const HBufC8 &aBuf)
	{
	TBool SingleTestResult 	= EFalse;
	TBool OverallTestResult = EFalse;
	ReadL( aCid );
	TInt length = iFields->Count();
	for( TInt i = 0; i < length; ++i )
		{
		if( (*iFields)[i].StorageType() == KStorageTypeStore )
			{
			TDesC8 *thething = (*iFields)[i].StoreStorage()->Thing();
			TESTPRINTI( SingleTestResult = ( thething != NULL ), i );
			OverallTestResult = SingleTestResult;
			TESTPRINTI( SingleTestResult = ( (*iFields)[i].StoreStorage()->Thing()->Compare(aBuf) == 0 ), i );
			OverallTestResult = OverallTestResult && SingleTestResult;
			}
		}
	CloseL( EFalse );
	return OverallTestResult;
	}

void CRandomBlobStep::BlobTestL()
	{
	TAutoClose< RPointerArray< HBufC8 > > blobs;
	
	//test for 0 bytes, empty descriptor fails
	blobs.iObj.AppendL( RandomBlobLC( 1 ) );//1 byte
	blobs.iObj.AppendL( RandomBlobLC( KKByte ) );//1 kbyte
	blobs.iObj.AppendL( RandomBlobLC( 10 * KKByte ) ); //10 kbytes
	blobs.iObj.AppendL( RandomBlobLC( 50 * KKByte ) ); //50 kbytes
	blobs.iObj.AppendL( RandomBlobLC( 100 * KKByte ) ); //100 kbytes
	
	TContactItemId cid = 0;
	TInt length = blobs.iObj.Count();
	TInt i = 0;
	
	iIterate->Reset();
	for(; i < length; ++i )
		{
		cid = iIterate->NextL();
		SetAllBlobsL( cid, *(blobs.iObj[i]) );
		}
	
	iIterate->Reset();
	for( i = 0; i < length; ++i )
		{
		cid = iIterate->NextL();
		TESTPRINTI( CheckAllBlobsL( cid, *(blobs.iObj[i]) ), i);
		}
		
	for( i = length - 1; i >= 0; --i )
		{
		CleanupStack::PopAndDestroy( blobs.iObj[i] );
		}
	}

//increments the size of blobs created by 1 kb until a failiure occurs
void CRandomBlobStep::MaxBlobTestL()
	{
	TBool SetSuccess = EFalse;
	HBufC8 *theblob = NULL;
	
	TContactItemId cid = iIterate->NextL();
	TInt i = 0;
	TInt err = KErrNone;

	for( i = 1; (i < KKByte); ++i )
		{
		theblob = RandomBlobLC( i * KKByte );
		TRAP( err, SetAllBlobsL( cid, *theblob ) );
		if( (KErrNone != err) &&  (KErrNoMemory != err) )
			{
			User::Leave( err );
			}
		else if( KErrNoMemory == err )
			{
			SetSuccess = EFalse;
			}
		else
			{
			TESTPRINTI( SetSuccess = CheckAllBlobsL( cid, *theblob ), i );
			}
			
		CleanupStack::PopAndDestroy( theblob );
		theblob = NULL;
		
		if( !SetSuccess )
			{
			break;
			}
		}
	
	_LIT(KMaxSizePrint,"Max size is %d KiloBytes");
	INFO_PRINTF2(KMaxSizePrint, SetSuccess ? i : --i);
	}


