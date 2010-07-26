/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include "t_lowdiskspace.h"

#include <cntfldst.h>

_LIT(KTestName, "T_LOWDISKSPACE");
_LIT( KFileName, "c:\\anyfile.any" );
_LIT(KDatabaseFileName,"C:T_LOWDISKSPACE");

GLDEF_D RTest test(KTestName);

static const TInt KMinusFull = 50000;
static const TInt64 KLowDiskThreshold = 0x20000;

const TInt KFiveHundredContactItems = 500; 

CLowDiskSpaceTest* CLowDiskSpaceTest::NewL()
	{
	CLowDiskSpaceTest* self = new(ELeave) CLowDiskSpaceTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CLowDiskSpaceTest::CLowDiskSpaceTest():CActive(EPriorityIdle), iCount(0), iManyFiles(0)
	{
	CActiveScheduler::Add(this);
	}

CLowDiskSpaceTest::~CLowDiskSpaceTest()
	{
	Cleanup();
	}

void CLowDiskSpaceTest::Cleanup()
	{
	TRAP_IGNORE(ClearDiskL() );

	delete iContactDatabase;
	iContactDatabase = NULL;
	
	iFileSession.Close();
	iFile->Close();
	delete iFile;
	iFile = NULL;

	TRAP_IGNORE(CContactDatabase::DeleteDatabaseL(KDatabaseFileName) );

	Cancel();
	}

void CLowDiskSpaceTest::DoCancel()
	{
	}

void CLowDiskSpaceTest::ConstructL()
	{
	iContactDatabase = CContactDatabase::ReplaceL(KDatabaseFileName);
	
	// populate the database with 500 contact items
	for (TInt i = 0; i < KFiveHundredContactItems; ++i)
		{
		AddContactToDatabaseL();
		}

	User::LeaveIfError(iFileSession.Connect());
	iFile = new(ELeave) RFile();
	}

void CLowDiskSpaceTest::AddContactToDatabaseL()
	{
	_LIT(KXChar, "x");
	CContactItem* item = CContactCard::NewLC();
	CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCompanyName);
	field->SetMapping(KUidContactFieldVCardMapORG);
	field->TextStorage()->SetTextL(KXChar() );
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(KXChar() );
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
 	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(KXChar() );
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
 	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->TextStorage()->SetTextL(KXChar() );
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPhoneNumber);
	field->SetMapping(KUidContactFieldVCardMapTEL);
 	field->SetHidden(ETrue);
	field->TextStorage()->SetTextL(KXChar() );
	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field = CContactItemField::NewLC(KStorageTypeText,KUidContactFieldSuffixName);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(KXChar() );
 	item->AddFieldL(*field);
	CleanupStack::Pop(); // field
	field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldPrefixName);
	field->SetMapping(KUidContactFieldVCardMapUnusedN);
	field->TextStorage()->SetTextL(KXChar() );
 	item->AddFieldL(*field);
	CleanupStack::Pop(); // field

	iContactDatabase->AddNewContactL(*item);
	CleanupStack::PopAndDestroy(); // item
	}


void CLowDiskSpaceTest::Start()
	{
	TRequestStatus *pS=&iStatus;
	User::RequestComplete(pS,KErrNone);
	SetActive();
	}

void CLowDiskSpaceTest::RunL()
	{
	TVolumeInfo tv;
	switch(iCount)
		{
	case EFillDisk:
	FillDiskL();//Fill the disk
	iCount++;
	Start();
	break;

	case EDatabaseTest:
	User::LeaveIfError( iFileSession.Volume(tv) );
	if(tv.iFree >= KLowDiskThreshold)
		{
		test.Printf(_L("Free space increased, refill the disk again.\n"));
		ClearDiskL();
		FillDiskL();//Fill the disk
		Start();
		break;
		}
	DatabaseTestL();
	iCount++;
	Start();
	break;

	case EClearDisk:
	ClearDiskL();
	iCount++;
	Start();
	break;

	default:
	CActiveScheduler::Stop();
	break;
		}
	}

void CLowDiskSpaceTest::FillDiskL()
	{
	_LIT(KFillDiskTitle, "Fill the disk");
	test.Next(_L("Fill the disk"));

	
	TVolumeInfo tv;
	User::LeaveIfError( iFileSession.Volume(tv) );
	
	TInt frees = 0;
	iManyFiles = tv.iFree / KMaxTInt;
	
	if ( iManyFiles > 0)
		{
		TPtrC tname( KFileName );
		TInt i = 0;
		for( ; i < iManyFiles; ++i )
			{
			HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
			TPtr fptr = fval->Des() ;
			fptr.Append( tname );
			fptr.AppendNum( i );
			
			User::LeaveIfError( iFile->Replace( iFileSession, fptr, EFileWrite ) );
			User::LeaveIfError( iFile->SetSize( KMaxTInt ) );
			iFile->Close();
			
			CleanupStack::PopAndDestroy( fval );
			}
		User::LeaveIfError( iFileSession.Volume(tv) );
		
		frees = tv.iFree - KMinusFull ;
		if( frees <= 0 )
			{
			frees = tv.iFree;
			}
		User::LeaveIfError( iFile->Replace( iFileSession, KFileName, EFileWrite ) );
		
	#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		TInt err = KErrDiskFull;
		while(err == KErrDiskFull)
			{
			err = iFile->SetSize(frees);
			frees -= 100;
			if(frees <= 0)
				{
				break;	
				}
			}
	#else
		User::LeaveIfError( iFile->SetSize( frees ) );
	#endif
	
		iFile->Close(); 
		}
	else
		{
		frees = tv.iFree - KMinusFull ;
		if( frees <= 0 )
			{
			frees = tv.iFree;
			}
		User::LeaveIfError( iFile->Replace( iFileSession, KFileName, EFileWrite ) );

	#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__
		TInt err = KErrDiskFull;
		while(err == KErrDiskFull)
			{
			err = iFile->SetSize(frees);
			frees -= 100;
			if(frees <= 0)
				{
				break;	
				}
			}
	#else
		User::LeaveIfError( iFile->SetSize( frees ) );
	#endif
				
		iFile->Close();
		}
	}


void CLowDiskSpaceTest::DatabaseTestL()
	{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	_LIT(KDatabaseTestTitle, "Add new contact and check whether disk is full");
#else
	_LIT(KDatabaseTestTitle, "Test add/delete contacts in low disk conditions");
#endif

	test.Next(_L("Test add/delete contacts in low disk conditions"));

	CContactCard* card=CContactCard::NewL();
	CleanupStack::PushL(card);
	TRAPD(ret, iContactDatabase->AddNewContactL(*card));
	if (ret != KErrDiskFull)
		{
		Cleanup(); // delete files filling up disk before exiting the test
		test(EFalse); // fail the test
		}

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__

	_LIT(KNumContactsMsg, "Number of contacts in the database: %d\n");
	_LIT(KDeletingMsg, "Deleting contact item: %d... ");
	_LIT(KPassMsg, " [PASS -- Contacts deleted]\n");
	_LIT(KFailMsg, " [FAILED with err code: %d]\n");

	test.Printf(KNumContactsMsg, iContactDatabase->CountL());
	const CContactIdArray* idArray = iContactDatabase->SortedItemsL(); // don't take ownership
	const TInt KCount = idArray->Count();
	test(KCount == KFiveHundredContactItems); // check we have all 500 items in the database


	iContactDatabase->DatabaseBeginLC(ETrue);
	for (TInt i = KCount - 1; i >= 0; --i)
		{
		const TContactItemId KItemId = (*idArray)[i];
		test.Printf(KDeletingMsg, KItemId);
		TRAP(ret, iContactDatabase->DeleteContactL(KItemId) );
		if (ret != KErrNone)
			{
			test.Printf(KFailMsg, ret);
			Cleanup(); // delete files filling up disk before exiting the test		
			test(EFalse); // fail the test
			}
		}	
	iContactDatabase->DatabaseCommitLP(ETrue);
	test.Printf(KPassMsg);

#endif

	CleanupStack::PopAndDestroy(card);
	}

void CLowDiskSpaceTest::ClearDiskL()
	{
	test.Next(_L("Clear the disk"));

	
	TPtrC tname( KFileName );
	TInt i = 0;
	for(i = 0 ; i < iManyFiles; ++i )
		{
		HBufC *fval=HBufC::NewLC( tname.Length()+4 );//assume #files < 10000
		TPtr fptr=fval->Des();
		fptr.Append( tname );
		fptr.AppendNum( i );
		TInt err = iFileSession.Delete( fptr );
		if( err != KErrNone && err != KErrNotFound )
			{
			User::Leave( err );
			}
		
		CleanupStack::PopAndDestroy( fval );
		}
	TInt err = iFileSession.Delete( KFileName );

	if( err != KErrNone && err != KErrNotFound )
		{
		User::Leave( err );
		}
	}

LOCAL_C void DoTestL()
	{
	CLowDiskSpaceTest* test = CLowDiskSpaceTest::NewL();
	test->Start();
	CActiveScheduler::Start();
	delete test;
	}

/**

@SYMTestCaseID     PIM-T-LOWDISKSPACE-0001

*/

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		test.Start(_L("@SYMTESTCaseID:PIM-T-LOWDISKSPACE-0001 T_LOWDISKSPACE"));

		TRAPD(r, DoTestL());
		test(r == KErrNone);
		test.End();
		test.Close();
		delete scheduler;	
		delete cleanup;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }

