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


#include <cntdb.h>
#include "nbcnttestlib.h"
#include "t_casynctest.h"


enum Tests
	{
	EOpenAndCancelPart1,
	EOpenAndCancelPart2,
	EOpenAndCancel,
	EOpenUnnamed,
	ETestOpenUnnamed,
	EOpenNamed,
	ETestOpenNamed
	};

namespace nsAsyncTest
	{
	_LIT(KFile,"C:CONTACTS.CDB");
	}



//===========================================================
// CAsyncTest Class
//===========================================================


CAsyncTest* CAsyncTest::NewLC()
	{
	CAsyncTest* self = new (ELeave) CAsyncTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CAsyncTest::~CAsyncTest()
	{
	delete iOpenOp;
	iTest->End();
	iTest->Close();
	delete iTest;		
	}
	
	
CAsyncTest::CAsyncTest():CActive(EPriorityIdle), iTestNo(0)
	{
	CActiveScheduler::Add(this);
	}

void CAsyncTest::ConstructL()
	{	
	TPtrC name(RThread().Name());
	iTest = new(ELeave) RTest(name);	
	iTest->Start(_L("T_CAsyncTest"));
	CreateNamedDbL();
	TRequestStatus *pS = &iStatus;
	User::RequestComplete(pS, KErrNone);
	SetActive();
	CActiveScheduler::Start();
	}


void CAsyncTest::CreateNamedDbL()
	{
	CContactDatabase* db = NULL;
	TRAPD(err, db = CContactDatabase::OpenL(nsAsyncTest::KFile));
	if (err == KErrNotFound)
		{
		TRAPD(err, db = CContactDatabase::ReplaceL(nsAsyncTest::KFile));
		if (err == KErrInUse)
			{
			User::After(1000);
			CreateNamedDbL();	
			}
		else
			{
			User::LeaveIfError(err);
			}
		}
		
	delete db;
	db = NULL;
	}

void CAsyncTest::DoCancel()
	{
	}

TInt CAsyncTest::RunError(TInt aError)
	{
	return aError;
	}
	

void CAsyncTest::RunL()
	{
	switch (iTestNo)
		{
		case EOpenAndCancelPart1:
			OpenUnnamedDatabaseTestL();
			User::After(100000); // Give other threads a chance to run during concurrent tests
			SetActive();
			++iTestNo;
			break;
		case EOpenAndCancelPart2:// Allow the database to open before cancelling
			CancelDatabaseTestL();
			NextTest();
			++iTestNo;
			break;
		case EOpenAndCancel:
			OpenAndCancelDatabaseTestL();
			SetActive();
			++iTestNo;
			break;
		case EOpenUnnamed:
			OpenUnnamedDatabaseTestL();
			SetActive();
			++iTestNo;
			break;
		case ETestOpenUnnamed:
			User::LeaveIfError(iStatus.Int());
			TestResultL();
			NextTest();
			++iTestNo;			
			break;			
		case EOpenNamed:
			User::LeaveIfError(iStatus.Int());
			OpenNamedDatabaseTestL();
			User::After(1000);
			SetActive();
			++iTestNo;			
			break;
		case ETestOpenNamed:
			{
			TestResultL();
			RThread thread;
			if(thread.RequestCount()!=0)
				User::Leave(KErrBadHandle);
			CActiveScheduler::Stop();
			iTest->Next(_L("Test Completed OK\n"));
			break;
			}
		default:
			ASSERT(EFalse);
			break;
		}
	}



void CAsyncTest::TestResultL()
	{
	CContactDatabase* db = iOpenOp->TakeDatabase();
	if(db == NULL)
		User::Leave(KErrNotFound);
	

	// Test adding a contact
	CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	matchAll->AddL(KUidContactFieldMatchAll);
	CContactItem* cntTemplate = db->ReadContactL(0, *matchAll);
	CleanupStack::PopAndDestroy(matchAll);
	CleanupStack::PushL(cntTemplate);

	CCntItemBuilder* cntItemBldr = CCntItemBuilder::NewLC(static_cast<CContactTemplate&>(*cntTemplate));
	CContactItem* tempCntItem = cntItemBldr->GetCntItemLC();
	db->AddNewContactL(*tempCntItem);

	CleanupStack::PopAndDestroy(tempCntItem);
	CleanupStack::PopAndDestroy(cntItemBldr);
	CleanupStack::PopAndDestroy(cntTemplate);
	delete iOpenOp;
	delete db;
	iOpenOp = NULL;
	db 		= NULL;
	
	iTest->Next(_L("Read data from Database\n"));
	
	}

void CAsyncTest::NextTest()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}


void CAsyncTest::OpenUnnamedDatabaseTestL()
	{
   	iTest->Next(_L("Open Unnamed Database\n"));
	iOpenOp = CContactDatabase::Open(iStatus, CContactDatabase::ESingleThread);
	}


void CAsyncTest::CancelDatabaseTestL()
	{
	iTest->Next(_L("Cancel Open Database\n"));
	delete iOpenOp;
	iOpenOp = NULL;
	}

void CAsyncTest::OpenAndCancelDatabaseTestL()
	{
	iTest->Next(_L("Open And Cancel Database\n"));
	iOpenOp = CContactDatabase::Open(iStatus, CContactDatabase::ESingleThread);
	delete iOpenOp;
	iOpenOp = NULL;
	}

void CAsyncTest::OpenNamedDatabaseTestL()
	{
   	iTest->Next(_L("Open Named Database\n"));
	iOpenOp = CContactDatabase::Open(nsAsyncTest::KFile, iStatus, CContactDatabase::ESingleThread);
	}




