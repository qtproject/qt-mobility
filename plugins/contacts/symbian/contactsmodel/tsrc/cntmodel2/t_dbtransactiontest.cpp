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


#include "t_dbtransactiontest.h"
_LIT(KTestName, "T_Database_Transaction_Tests"); // Used by Testhelper - gives an warning under ARMv5
const TInt MAX_TRY = 10;
#include "..\..\tsrc\testhelpers.h"
#include <e32base.h>

#include "t_casynctest.h"

/*
					How to Add a new concurrent Test
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Add a new header and cpp file for each new test class.
	Additions to the CConcurrentController
		1. Add a new private Method called Start-TestName
		2. Call the new method from StartTestL	
		3. Add a new enum value called E-TestName to TTestCodes in 
			the header file.
		4. Within the new Start-TestName method, call RunTestThreadL passing in
			the new enum. Refer to StartTimeOutTestL
	Additions to the CConcurrentTester
		1. Instantiate and execute your test class in the RunTestL method.
	
*/

CConcurrentController::~CConcurrentController()
	{
	iStore.ResetAndDestroy();
	}
	
CConcurrentController* CConcurrentController::NewLC()
	{
	CConcurrentController* self = new (ELeave) CConcurrentController();
	CleanupStack::PushL(self);
	return self;	
	}
	
CConcurrentController::CConcurrentController()
	{}
	

void CConcurrentController::StartTestL(TInt aNoOfTesters)
	{
//	StartTransactionTestL(aNoOfTesters);
	StartTimeOutTestL(aNoOfTesters);
//	StartAsyncOpenTestL(aNoOfTesters);
	}
	
void CConcurrentController::StartTransactionTestL(TInt aNoOfTesters)
	{
	test.Printf(_L("Starting Transaction Tests"));
	CreateTestersL(aNoOfTesters);
	// Create the tests and kick off the thread.
	for (TInt ii = 0; ii < aNoOfTesters; ++ii)
		{
		iStore[ii]->RunTestThreadL(); 
		}
	
	while (!Completed())	
		User::WaitForAnyRequest();
	}

void CConcurrentController::CreateTestersL(TInt aNoOfTesters)
	{
	if (iStore.Count() == 0)
		{
		_LIT(KThreadName,"Thread_%d");
		TBuf<256> buf;

		for (TInt ii = 0; ii < aNoOfTesters; ++ii)
			{
			buf.Format(KThreadName, ii);
			iStore.AppendL(CConcurrentTester::NewL(CEventResponse::NewL(*this), buf));
			}
		}
	}

void CConcurrentController::StartAsyncOpenTestL(TInt aNoOfTesters)
	{
	test.Printf(_L("Starting Async Open Tests"));
	CreateTestersL(aNoOfTesters);
	// Create the tests and kick off the thread.
	for (TInt ii = 0; ii < aNoOfTesters; ++ii)
		{
		iStore[ii]->RunTestThreadL(EAsyncOpenTest); 
		}
	
	while (!Completed())	
		User::WaitForAnyRequest();
	
	}

void CConcurrentController::StartTimeOutTestL(TInt aNoOfTesters)
	{
	test.Printf(_L("Starting TimeOut Test\n\r"));
	CreateTestersL(aNoOfTesters);
	
	for (TInt ii = 0; ii < aNoOfTesters; ++ii)
		{
		if (ii%2 == 0)
			{
			iStore[ii]->RunTestThreadL(ENonTimeOutTest);	
			}
		else
			{
			iStore[ii]->RunTestThreadL(ETimeOutTest);				
			}
		}
	while (!Completed())	
		User::WaitForAnyRequest();
	}


TBool CConcurrentController::Completed()
	{
	TInt noOfTesters = iStore.Count();
	for (TInt ii = 0; ii < noOfTesters; ++ii)
		{
		if (iStore[ii]->IsCompleted())
			return EFalse;	
		}
	return ETrue;
	}
	
void CConcurrentController::PublishError(TInt aError)
	{
	test.Printf(_L(" ->  TEST FAILED - Error %d was reported during execution"), aError);
	}





CEventResponse* CEventResponse::NewL(CConcurrentController& aController)
	{
	CEventResponse* self = new (ELeave) CEventResponse(aController);
	return self;	
	}


CEventResponse::CEventResponse(CConcurrentController& aController) : CActive(EPriorityIdle), iController(aController) 
	{
	CActiveScheduler::Add(this);
	}


CEventResponse::~CEventResponse()
	{}


void CEventResponse::RunL()
	{
	test.Next(_L("-> Thread Finished\r"));

	}


void CEventResponse::DoCancel()
	{
	Cancel();
	}


TInt CEventResponse::RunError(TInt aError)
	{
	iController.PublishError(aError);
	return aError;
	}


void CEventResponse::CompleteRequest(TInt aError)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aError);
	}
			
TRequestStatus& CEventResponse::RequestStatus()
	{
	return iStatus;
	}
			

CConcurrentTester::~CConcurrentTester()
	{
	delete iResponse;	
	iThread.Close();	
	}


CConcurrentTester* CConcurrentTester::NewL(CEventResponse* aResponse, const TDesC& aThreadName)
	{
	CConcurrentTester* self = new (ELeave) CConcurrentTester(aResponse, aThreadName);
	return self;	
	}

	
TInt CConcurrentTester::ThreadFunction(TAny* aTester)
	{
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;
	
	CActiveScheduler*  scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	TRAPD(err, static_cast<CConcurrentTester*>(aTester)->RunTestL());	

	delete scheduler;	
	scheduler = NULL;
    delete cleanup;
    cleanup   = NULL;
    
	return err;
	}

TBool CConcurrentTester::IsCompleted()
	{
	return iResponse->RequestStatus() == KRequestPending;	
	}

// Wrapper method for Test class RunTestL()
// This method is called from within the ThreadFunction() method
// and runs in a new thread. Apart from this function all the concurrent
// logic is in the CTransactionThreadTest class.

void CConcurrentTester::RunTestL()
	{
	switch(iTestCode)
		{
		case ETransactionTest:
			{
			CTransactionThreadTest* transTest = CTransactionThreadTest::NewLC();
			transTest->RunTestL();
			CleanupStack::PopAndDestroy(transTest);
			transTest = NULL;
			break;
			}
		case ETimeOutTest:
			{
			CTransTimeOutThread* timeOut = CTransTimeOutThread::NewLC();
			timeOut->RunTestL();
			CleanupStack::PopAndDestroy(timeOut);
			timeOut = NULL;	
			break;
			}
		case ENonTimeOutTest:
			{
			CReqTimeOutThread* nonTimeOut = CReqTimeOutThread::NewLC();
			nonTimeOut->RunTestL();
			CleanupStack::PopAndDestroy(nonTimeOut);
			nonTimeOut = NULL;
			break;
			}
		case EAsyncOpenTest:
			{
			CAsyncTest* asyncTest = CAsyncTest::NewLC();
			CleanupStack::PopAndDestroy(asyncTest);
			asyncTest = NULL;
			break;
			}
		default:
			{
			User::Leave(KErrNotFound);
			}	
		}
	}
	

void CConcurrentTester::RunTestThreadL(TInt aTestCode)
	{
	iTestCode = aTestCode;
	test.Next(_L("-> %s Started \r"));

	User::LeaveIfError(iThread.Create(iThreadName, CConcurrentTester::ThreadFunction, KDefaultStackSize, 0x2000, 0x200000, this, EOwnerThread));
	iThread.SetPriority(EPriorityMuchLess);
	iThread.Logon (iResponse->RequestStatus());
	iThread.Resume();
	}



	


/////////////////////////////////////////////////////////////////
// CTransactionThreadTest
CTransactionThreadTest::~CTransactionThreadTest()
	{
	delete iMatchAll;
	delete iCntItemBldr;
	delete iDB;
	delete iGoldenTemplate;
	delete iTest;
	}
	
CTransactionThreadTest* CTransactionThreadTest::NewLC()
	{
	CTransactionThreadTest* self = new (ELeave) CTransactionThreadTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
	
const CContactTemplate&  CTransactionThreadTest::GetSysTemplateL()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iDB->ReadContactL(iDB->TemplateId(), *matchAll));
		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	}
	


void CTransactionThreadTest::CreateAndCommitTestL()
	{
	iTest->Next((_L(" %s -> Create Contact and Commit Test"), RThread().Name() ));
	TInt cntID = AddAndCommitContactL();
	
	if (IsContactInDbL(cntID))
		{
		iTest->Next((_L(" %s  -> TEST SUCCESS - Contact FOUND after commit test"), RThread().Name() ));
		return;
		}
	iTest->Next((_L(" %s  -> TEST FAILED - Contact NOT FOUND after commit test"), RThread().Name() ));
	}



void CTransactionThreadTest::RollbackAndRecoverTestL()
	{
	iTest->Next((_L(" %s -> Rollback & Recover Test"), RThread().Name() ));
	iDB->DatabaseBeginLC(EFalse); 

	TInt cntID = AddContactL();
	
	iDB->DatabaseRollback();
	iDB->RecoverL();
	
	CleanupStack::Pop();// Need to pop the rollback
		
	if (!IsContactInDbL(cntID))
		{
		iTest->Next((_L(" %s -> TEST SUCCESS - Contact was NOT Found after Rollback test"), RThread().Name() ));
		return;
		}
	iTest->Next((_L(" %s -> TEST FAILED - Contact WAS Found after ROLLBACK test"), RThread().Name() ));
	}


	
void CTransactionThreadTest::UpdateAndCommitTestL()
	{
	iTest->Next((_L(" %s  -> Update Contact and Commit Test"), RThread().Name() ));

	TInt cntID = AddAndCommitContactL();

	iDB->DatabaseBeginL(EFalse);	
	UpdateContactL(cntID);
	iDB->DatabaseCommitL(EFalse);
	
	if (CheckChangedFieldL(cntID))
		{
		iTest->Next((_L(" %s -> TEST SUCCESS - Contact FOUND after Update commit test"), RThread().Name() ));
		return;
		}
	iTest->Next((_L(" %s  -> TEST FAILED - Contact NOT FOUND after Update commit test"), RThread().Name() ));
	}


void CTransactionThreadTest::DeleteAndCommitTestL()
	{
	iTest->Next((_L(" %s  -> Contact and Commit Test"), RThread().Name() ));

	TInt cntID = AddAndCommitContactL();

	iDB->DatabaseBeginL(EFalse);	
	DeleteContactL(cntID);
	iDB->DatabaseCommitL(EFalse);
	
	if (!IsContactInDbL(cntID))
		{
		iTest->Next((_L(" %s  -> TEST SUCCESS - Contact FOUND after Update commit test"), RThread().Name() ));
		return;
		}
	iTest->Next((_L(" %s  -> TEST FAILED - Contact NOT FOUND after Update commit test"), RThread().Name() ));
	}



// ----------- Helper methods -----------

TInt CTransactionThreadTest::AddContactL()
	{
	// Create the contact
	CContactItem* cntItem = iCntItemBldr->GetCntItemLC();    

    TInt cntID = 0;
    TRAP_IGNORE(cntID = iDB->AddNewContactL(*cntItem));
		

	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	return cntID;
	}


TInt CTransactionThreadTest::AddAndCommitContactL()
	{
	iDB->DatabaseBeginL(EFalse);
	TInt cntID = AddContactL();

	for (TInt ii = 0; ii < 100; ++ii)
	 	AddContactL();

	iDB->DatabaseCommitL(EFalse);
	return cntID;
	}


void CTransactionThreadTest::DeleteContactL(TInt aCntID) 
	{
	// Remove the contact
    iDB->DeleteContactL(aCntID);
	}


void CTransactionThreadTest::UpdateContactL(TInt aCntID)
	{
	CContactItem* cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
	CCntItemModifier* cntModifier = CCntItemModifier::NewLC();
	
	cntModifier->SetTextFieldL(KUidContactFieldGivenName, nsPlTransMgr::KName,
				 *cntItem);

	cntModifier->SetTextFieldL(KUidContactFieldPhoneNumber,
				KUidContactFieldVCardMapWORK, 
				KUidContactFieldVCardMapVOICE,
				KUidContactFieldVCardMapCELL,
		 		nsPlTransMgr::KNumber, *cntItem);

	cntModifier->SetTextFieldL(KUidContactFieldEMail,
			    KUidContactFieldVCardMapWORK,
			    nsPlTransMgr::KEmail, *cntItem);


	cntModifier->SetTextFieldL(KUidContactFieldAddress, 
				nsPlTransMgr::KHomeAdd, *cntItem);

	CleanupStack::PopAndDestroy(cntModifier);

	CContactItem* cntItem2 = iDB->UpdateContactLC(cntItem->Id(), cntItem);
	CleanupStack::PopAndDestroy(cntItem2);
	CleanupStack::PopAndDestroy(cntItem);
	}



TBool CTransactionThreadTest::CheckChangedFieldL(TInt aCntID)
	{
	CContactItem* cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
	CCntItemModifier* cntModifier = CCntItemModifier::NewLC();

	TBool retval = cntModifier->FindTextFieldL(KUidContactFieldGivenName, nsPlTransMgr::KName,
				 *cntItem) & 
		cntModifier->FindTextFieldL(KUidContactFieldPhoneNumber,
				KUidContactFieldVCardMapWORK, 
				KUidContactFieldVCardMapVOICE,
				KUidContactFieldVCardMapCELL,
		 		nsPlTransMgr::KNumber, *cntItem) &
		cntModifier->FindTextFieldL(KUidContactFieldAddress, 
				nsPlTransMgr::KHomeAdd, *cntItem)&
		cntModifier->FindTextFieldL(KUidContactFieldEMail,
			    KUidContactFieldVCardMapWORK,
			    nsPlTransMgr::KEmail, *cntItem); 

	CleanupStack::PopAndDestroy(cntModifier);
	CleanupStack::PopAndDestroy(cntItem);
	return retval;
	}



TBool CTransactionThreadTest::IsContactInDbL(TInt aCntID)
	{
	CContactItem* cntItem = NULL;
	TRAPD(err, cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
		CleanupStack::PopAndDestroy(cntItem);
		cntItem = NULL;
		);
	
	if (err == KErrNotFound)
		{
		return FALSE;	
		}
	User::LeaveIfError(err);
	return TRUE;
	}


void CTransactionThreadTest::ConstructL()
	{
	iDB = CContactDatabase::OpenL(); // Default Database.
	const CContactTemplate& sysTempl(GetSysTemplateL() );
	iCntItemBldr = CCntItemBuilder::NewLC(sysTempl);
	CleanupStack::Pop(iCntItemBldr);
		
	// Create and name an RTest
	TPtrC name(RThread().Name());
	iTest = new(ELeave) RTest(name);
	}


void CTransactionThreadTest::RunTestL()
	{

	for (int testIndex = 0; testIndex <= EUpdateAndCommitTest; ++testIndex)
		{
		switch(testIndex)	
			{
			case ECreateAndCommitTest:
				{
				CreateAndCommitTestL();
				break;
				}
			case ERollbackAndRecoverTest:
				{
				RollbackAndRecoverTestL();
				break;
				}
			case EDeleteAndCommitTest:
				{
				DeleteAndCommitTestL();
				break;
				}
			case EUpdateAndCommitTest:
				{
				UpdateAndCommitTestL();
				break;
				}
			
			default:
				{
				User::Leave(KErrNotFound);
				}
			}
		}
	}


	



// Request Timeout test classes
CTransTimeOutThread* CTransTimeOutThread::NewLC()
	{
	CTransTimeOutThread* self = new (ELeave) CTransTimeOutThread();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;					
	}
CTransTimeOutThread::~CTransTimeOutThread()
	{
	delete iDB;
	delete iTest;		
	}
	
void CTransTimeOutThread::RunTestL()
	{
	// Just wait for TimeOut
	iDB->DatabaseBeginL(EFalse);

	iTest->Next(_L("Started Transaction & Waiting for 60 seconds...\n"));
	User::After(KSixtySecondsTimeOut);
	iTest->Next(_L("Timed Out...\n"));

	TRAP_IGNORE(iDB->DatabaseCommitL(EFalse));

	}
	
void CTransTimeOutThread::ConstructL()
	{
	iDB = CContactDatabase::OpenL(); // Default Database.

	// Create and name an RTest
	TPtrC name(RThread().Name());
	iTest = new(ELeave) RTest(name);	
	}




	
	
	

////////////




CReqTimeOutThread* CReqTimeOutThread::NewLC()
	{
	CReqTimeOutThread* self = new (ELeave) CReqTimeOutThread();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;			
	}

CReqTimeOutThread::~CReqTimeOutThread()
	{
	delete iMatchAll;
	delete iCntItemBldr;
	delete iDB;
	delete iGoldenTemplate;
	delete iTest;
	}

void CReqTimeOutThread::RunTestL()
	{
	TTime finishTime;
	TTime now;
	finishTime.UniversalTime();
	now.UniversalTime();
	finishTime +=(KSixtySecondsTimeOut);
	
	while (now < finishTime)							
		{
		AddContactL(); // Contact the server
		iTest->Printf(_L("Non TimeOut Thread is maintaining contact with the server\n\r"));
		User::After(KOneHalfSecondTimeOut);// Every .25 seconds
		now.UniversalTime();
		}
	}

void CReqTimeOutThread::ConstructL()
	{
	iDB = CContactDatabase::OpenL(); // Default Database.
	const CContactTemplate& sysTempl(GetSysTemplateL() );
	iCntItemBldr = CCntItemBuilder::NewLC(sysTempl);
	CleanupStack::Pop(iCntItemBldr);
		
	// Create and name an RTest
	TPtrC name(RThread().Name());
	iTest = new(ELeave) RTest(name);	
	}

void CReqTimeOutThread::AddContactL()
	{
	// Create the contact
	CContactItem* cntItem = iCntItemBldr->GetCntItemLC();    

    TRAPD(err, iDB->AddNewContactL(*cntItem));
	if(err != KErrNotReady)
		{
		User::LeaveIfError(err); // Propagate the error
		}
		
	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	}

const CContactTemplate& CReqTimeOutThread::GetSysTemplateL()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iDB->ReadContactL(iDB->TemplateId(), *matchAll));
		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	}
	








































CDBTransTest* CDBTransTest::NewLC()
	{
	CDBTransTest* self = new (ELeave) CDBTransTest();
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;	
	}



void CDBTransTest::ConstructL()
	{
	iDB = CContactDatabase::ReplaceL(); // Default Database.
	const CContactTemplate& sysTempl(GetSysTemplateL() );
	iCntItemBldr = CCntItemBuilder::NewLC(sysTempl);
	CleanupStack::Pop(iCntItemBldr);
	}


const CContactTemplate&  CDBTransTest::GetSysTemplateL()
	{
	if (!iGoldenTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
		matchAll->AddL(KUidContactFieldMatchAll);

		iGoldenTemplate = static_cast<CContactTemplate*>(iDB->ReadContactL(iDB->TemplateId(), *matchAll));
		CleanupStack::PopAndDestroy(matchAll);
		}
	return *iGoldenTemplate;
	}
	
	

CDBTransTest::~CDBTransTest()
	{
	delete iMatchAll;
	delete iCntItemBldr;
	delete iDB;
	delete iGoldenTemplate;
	}



void CDBTransTest::RunTestsL()
	{
	UpdateAndCommitTestL();
	CreateAndCommitTestL(); 
	DeleteAndCommitTestL();
	RollbackAndRecoverTestL();
	OutOfMemoryUpdateTestL();
	OutOfMemoryDeleteTestL();
	OutOfMemoryAddTestL();
	}

// ---------- Tests ---------

// Create/commit & Rollback/recover tests.

void CDBTransTest::CreateAndCommitTestL()
	{
	test.Next(_L("-> Create Contact and Commit Test"));

	TInt cntID = AddAndCommitContactL();
	
	if (IsContactInDbL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact FOUND after commit test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact NOT FOUND after commit test"));

	}

void CDBTransTest::RollbackAndRecoverTestL()
	{
	test.Next(_L("-> Rollback & Recover Test"));

	iDB->DatabaseBeginLC(EFalse); 

	TInt cntID = AddContactL();
	
	iDB->DatabaseRollback();
	iDB->RecoverL();
	
	CleanupStack::Pop();// Need to pop the rollback
		
	if (!IsContactInDbL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact was NOT Found after Rollback test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact WAS Found after ROLLBACK test"));

	}

void CDBTransTest::UpdateAndCommitTestL()
	{
	test.Next(_L("-> Update Contact and Commit Test"));


	TInt cntID = AddAndCommitContactL();

	iDB->DatabaseBeginL(EFalse);	
	UpdateContactL(cntID);
	iDB->DatabaseCommitL(EFalse);
	
	if (CheckChangedFieldL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact FOUND after Update commit test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact NOT FOUND after Update commit test"));

	}


void CDBTransTest::DeleteAndCommitTestL()
	{
	test.Next(_L("-> Contact and Commit Test"));


	TInt cntID = AddAndCommitContactL();

	iDB->DatabaseBeginL(EFalse);	
	DeleteContactL(cntID);
	iDB->DatabaseCommitL(EFalse);
	
	if (!IsContactInDbL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact FOUND after Update commit test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact NOT FOUND after Update commit test"));

	}



// Out Of Memory/Rollback/Recover Tests

void CDBTransTest::OutOfMemoryAddTestL()
	{

	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	TInt index =0;
	
	while (ret!=KErrNone && index < MAX_TRY)
		{
		++index;
		// Add a successful contact, guess the next contact id
		TInt cntID = AddAndCommitContactL();
		++cntID;
		iDB->DatabaseBeginL(EFalse);	
		++failAt;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, AddContactL());
		if (err==KErrNone)
			{
			iDB->DatabaseCommitL(EFalse);
			test.Next(_L("-> TEST SUCCESS - Contact committed during OOM Test"));

			__UHEAP_RESET;
			return;
			}
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iDB->DatabaseRollback();
			iDB->RecoverL();
			if (!IsContactInDbL(cntID))
				{
				test.Printf(_L(" ->Correct result - Create rollback during OOM - Contact NOT Found... Step: %d \n\r"),failAt); 
				}
				else
				{
				test.Printf(_L(" ->Error - Delete rollback during OOM - Contact Found... Step: %d \n\r"),failAt);
				User::Leave(KErrNotFound);
				}
			}
		}
		if (index == MAX_TRY)
			{
			test.Printf(_L(" ->Gave up after %d attempts\n\r"),index); 
			}
	}


void CDBTransTest::OutOfMemoryDeleteTestL()
	{
	TInt ret	= KErrNoMemory;
	TInt failAt = 0;	  
	TInt index =0;
	
	while (ret!=KErrNone && index < MAX_TRY)
		{
		++index;
		++failAt;
		TInt cntID = AddAndCommitContactL();
		iDB->DatabaseBeginL(EFalse);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, DeleteContactL(cntID));
		
		if (err==KErrNone)
			{
			iDB->DatabaseCommitL(EFalse);
			test.Next(_L("-> TEST SUCCESS - Delete Contact committed during OOM Test"));

			__UHEAP_RESET;
			return;
			}
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iDB->DatabaseRollback();
			iDB->RecoverL();
		
			if (IsContactInDbL(cntID))
				{
				test.Printf(_L(" ->Correct result - Delete rollback during OOM - Contact Found... Step: %d \n\r"),failAt); 
				}
				else
				{
				test.Printf(_L(" ->Error - Delete rollback during OOM - Contact NOT Found... Step: %d \n\r"),failAt);
				User::Leave(KErrNotFound);
				}
			}
		}
		if (index == MAX_TRY)
			{
			test.Printf(_L(" ->Gave up after %d attempts\n\r"),index); 
			}
	}


void CDBTransTest::OutOfMemoryUpdateTestL()
	{

	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	TInt index =0;
	
	while (ret!=KErrNone && index < MAX_TRY)
		{
		++index;
		TInt cntID = AddAndCommitContactL();
		iDB->DatabaseBeginL(EFalse);
		++failAt;

		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, UpdateContactL(cntID));
		if (err==KErrNone)
			{
			iDB->DatabaseCommitL(EFalse);
			test.Next(_L("-> TEST SUCCESS - Contact committed during OOM Test"));

			__UHEAP_RESET;
			return;
			}
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iDB->DatabaseRollback();
			iDB->RecoverL();
			
			// Has the contact changed?
			if (!CheckChangedFieldL(cntID))
				{
				test.Printf(_L(" ->Correct result - Update rollback during OOM - Updated Contact NOT Found... Step: %d \n\r"),failAt);			
				}
				else
				{
				test.Printf(_L(" ->Error - Update rollback during OOM - Updated Contact Found... Step: %d \n\r"),failAt);
				User::Leave(KErrNotFound);
				}
			}
		}
		if (index == MAX_TRY)
			{
			test.Printf(_L(" ->Gave up after %d attempts\n\r"),index); 
			}

	}



// ----------- Helper methods -----------

TInt CDBTransTest::AddContactL()
	{
	// Create the contact
	CContactItem* cntItem = iCntItemBldr->GetCntItemLC();    

    TInt cntID = iDB->AddNewContactL(*cntItem);

	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	return cntID;
	}


TInt CDBTransTest::AddAndCommitContactL()
	{
	iDB->DatabaseBeginL(EFalse);
	TInt cntID = AddContactL();
	iDB->DatabaseCommitL(EFalse);
	return cntID;
	}


void CDBTransTest::DeleteContactL(TInt aCntID) 
	{
	// Remove the contact
    iDB->DeleteContactL(aCntID);
	}


void CDBTransTest::UpdateContactL(TInt aCntID)
	{
	CContactItem* cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
	CCntItemModifier* cntModifier = CCntItemModifier::NewLC();
	
	cntModifier->SetTextFieldL(KUidContactFieldGivenName, nsPlTransMgr::KName,
				 *cntItem);

	cntModifier->SetTextFieldL(KUidContactFieldPhoneNumber,
				KUidContactFieldVCardMapWORK, 
				KUidContactFieldVCardMapVOICE,
				KUidContactFieldVCardMapCELL,
		 		nsPlTransMgr::KNumber, *cntItem);

	cntModifier->SetTextFieldL(KUidContactFieldEMail,
			    KUidContactFieldVCardMapWORK,
			    nsPlTransMgr::KEmail, *cntItem);


	cntModifier->SetTextFieldL(KUidContactFieldAddress, 
				nsPlTransMgr::KHomeAdd, *cntItem);

	CleanupStack::PopAndDestroy(cntModifier);

	CContactItem* cntItem2 = iDB->UpdateContactLC(cntItem->Id(), cntItem);
	CleanupStack::PopAndDestroy(cntItem2);
	CleanupStack::PopAndDestroy(cntItem);
	}



TBool CDBTransTest::CheckChangedFieldL(TInt aCntID)
	{
	CContactItem* cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
	CCntItemModifier* cntModifier = CCntItemModifier::NewLC();

	TBool retval = cntModifier->FindTextFieldL(KUidContactFieldGivenName, nsPlTransMgr::KName,
				 *cntItem) & 
		cntModifier->FindTextFieldL(KUidContactFieldPhoneNumber,
				KUidContactFieldVCardMapWORK, 
				KUidContactFieldVCardMapVOICE,
				KUidContactFieldVCardMapCELL,
		 		nsPlTransMgr::KNumber, *cntItem) &
		cntModifier->FindTextFieldL(KUidContactFieldAddress, 
				nsPlTransMgr::KHomeAdd, *cntItem)&
		cntModifier->FindTextFieldL(KUidContactFieldEMail,
			    KUidContactFieldVCardMapWORK,
			    nsPlTransMgr::KEmail, *cntItem); 

	CleanupStack::PopAndDestroy(cntModifier);
	CleanupStack::PopAndDestroy(cntItem);
	return retval;
	}



TBool CDBTransTest::IsContactInDbL(TInt aCntID)
	{
	CContactItem* cntItem = NULL;
	TRAPD(err, cntItem = iDB->ReadContactLC(aCntID, *iMatchAll);
		CleanupStack::PopAndDestroy(cntItem);
		cntItem = NULL;
		);
	
	if (err == KErrNotFound)
		{
		return FALSE;	
		}
	User::LeaveIfError(err);
	return TRUE;
	}

/**

@SYMTestCaseID     PIM-T-DBTRANSACTIONTEST-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L("@SYMTESTCaseID:PIM-T-DBTRANSACTIONTEST-0001 ----------- Concurrent tests ----------"));


//	CDBTransTest* tranTest = CDBTransTest::NewLC();	
//	tranTest->RunTestsL();
	CConcurrentController* testController = CConcurrentController::NewLC();
	testController->StartTestL(6);

	test.End();
	CleanupStack::PopAndDestroy(testController);
//	CleanupStack::PopAndDestroy(tranTest);
    CleanupStack::Pop(); // test.Close & tranMgrTest
	}




GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	CActiveScheduler* scheduler=new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,DoTestsL());	
			__ASSERT_ALWAYS(err == KErrNone, User::Panic(_L("DBDumper Failed"),err) ); 
			delete cleanup;
			}
		delete scheduler;
		}
	test.Close();	
	
	__UHEAP_MARKEND;
	return KErrNone;
	
	}



