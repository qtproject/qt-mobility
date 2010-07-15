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


#include "t_pltransactionmanager.h"
_LIT(KTestName, "T_PlTransactionManager"); // Used by Testhelper - gives an warning under ARMv5
#include "../../tsrc/testhelpers.h"
#include <cntdef.h>


CPlTransMgrTest* CPlTransMgrTest::NewLC()
	{
	CPlTransMgrTest* self = new (ELeave) CPlTransMgrTest();
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;	
	}



void CPlTransMgrTest::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iPersistLayer = CPersistenceLayer::NewLC(iFs);
	CleanupStack::Pop(iPersistLayer); 

	MLplContactsFile& cntFile = iPersistLayer->ContactsFileL();
	// Empty loops required to remove winscw warning
	cntFile.CreateL(nsPlTransMgr::KFilename, MLplContactsFile::EPlOverwrite);
	while(cntFile.OpenStepL   (nsPlTransMgr::KFilename)){};

	iCntItemBldr = CCntItemBuilder::NewLC(iPersistLayer->ContactProperties().SystemTemplateL());
	CleanupStack::Pop(iCntItemBldr);

	iMatchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
										   CContactItemViewDef::EIncludeHiddenFields);
	iMatchAll->AddL(KUidContactFieldMatchAll);
	CleanupStack::Pop(iMatchAll);
	}



CPlTransMgrTest::~CPlTransMgrTest()
	{
	delete iMatchAll;
	delete iCntItemBldr;
	delete iPersistLayer;
	iFs.Close();
	}



void CPlTransMgrTest::RunTestsL()
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

void CPlTransMgrTest::CreateAndCommitTestL()
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

void CPlTransMgrTest::RollbackAndRecoverTestL()
	{
	test.Next(_L("-> Rollback & Recover Test"));

	iPersistLayer->TransactionManager().StartTransactionL();

	TInt cntID = AddContactL();
	
	iPersistLayer->TransactionManager().RollbackCurrentTransactionL(0);
  	// Close the database to allow recovery
  	iPersistLayer->ContactsFileL().Close();
  		
	iPersistLayer->ContactsFileL().RecoverL(nsPlTransMgr::KFilename);
	
	if (!IsContactInDbL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact was NOT Found after Rollback test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact WAS Found after ROLLBACK test"));

	}


void CPlTransMgrTest::UpdateAndCommitTestL()
	{
	test.Next(_L("-> Update Contact and Commit Test"));


	TInt cntID = AddAndCommitContactL();

	iPersistLayer->TransactionManager().StartTransactionL();	
	UpdateContactL(cntID);
	iPersistLayer->TransactionManager().CommitCurrentTransactionL(0);
	
	if (CheckChangedFieldL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact FOUND after Update commit test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact NOT FOUND after Update commit test"));

	}


void CPlTransMgrTest::DeleteAndCommitTestL()
	{
	test.Next(_L("-> Contact and Commit Test"));


	TInt cntID = AddAndCommitContactL();

	iPersistLayer->TransactionManager().StartTransactionL();	
	DeleteContactL(cntID);
	iPersistLayer->TransactionManager().CommitCurrentTransactionL(0);
	
	if (!IsContactInDbL(cntID))
		{
		test.Next(_L("-> TEST SUCCESS - Contact FOUND after Update commit test"));

		return;
		}
	test.Next(_L("-> TEST FAILED - Contact NOT FOUND after Update commit test"));

	}



// Out Of Memory/Rollback/Recover Tests

void CPlTransMgrTest::OutOfMemoryAddTestL()
	{

	TInt ret=KErrNoMemory;
	TInt failAt=0;	  

	while (ret!=KErrNone)
		{
		// Add a successful contact, guess the next contact id
		TInt cntID = AddAndCommitContactL();
		++cntID;
		iPersistLayer->TransactionManager().StartTransactionL();
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, AddContactL());
		if (err==KErrNone)
			{
			TRAP(err, iPersistLayer->TransactionManager().CommitCurrentTransactionL(0));
			if (err==KErrNone)
				{
				test.Next(_L("-> TEST SUCCESS - Contact committed during OOM Test"));

				__UHEAP_RESET;
				return;
				}
			}
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iPersistLayer->TransactionManager().RollbackCurrentTransactionL(0);
			 	// Close the database to allow recovery
  			iPersistLayer->ContactsFileL().Close();
			iPersistLayer->ContactsFileL().RecoverL(nsPlTransMgr::KFilename);
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
	}


void CPlTransMgrTest::OutOfMemoryDeleteTestL()
	{
	TInt ret	= KErrNoMemory;
	TInt failAt = 0;	  

	while (ret!=KErrNone)
		{
		failAt++;
		TInt cntID = AddAndCommitContactL();
		iPersistLayer->TransactionManager().StartTransactionL();
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, DeleteContactL(cntID));
		
		if (err==KErrNone)
			{
			TRAP(err, iPersistLayer->TransactionManager().CommitCurrentTransactionL(0));
			if (err==KErrNone)
				{
				test.Next(_L("-> TEST SUCCESS - Delete Contact committed during OOM Test"));

				__UHEAP_RESET;
				return;
				}
			}
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iPersistLayer->TransactionManager().RollbackCurrentTransactionL(0);
			// Close the database to allow recovery
  			iPersistLayer->ContactsFileL().Close();
			iPersistLayer->ContactsFileL().RecoverL(nsPlTransMgr::KFilename);
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
	}


void CPlTransMgrTest::OutOfMemoryUpdateTestL()
	{

	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	
	iTestNo = 0;
	while (ret!=KErrNone)
		{
		++iTestNo;
		//if (iTestNo == 790)
		//	TInt i = 0;
		TInt cntID = AddAndCommitContactL();
		iPersistLayer->TransactionManager().StartTransactionL();
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAPD(err, UpdateContactL(cntID));
		if (err==KErrNone)
			{
			TRAP(err, iPersistLayer->TransactionManager().CommitCurrentTransactionL(0));
			if (err==KErrNone)
				{
				test.Next(_L("-> TEST SUCCESS - Contact committed during OOM Test"));

				__UHEAP_RESET;
				return;
				}
			}
			
		__UHEAP_RESET;
		if (err!=KErrNoMemory && err!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),err);
			}
		if (err == KErrNoMemory)
			{
			iPersistLayer->TransactionManager().RollbackCurrentTransactionL(0);
			// Close the database to allow recovery
  			iPersistLayer->ContactsFileL().Close();
			iPersistLayer->ContactsFileL().RecoverL(nsPlTransMgr::KFilename);
			
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
	}



// ----------- Helper methods -----------

TInt CPlTransMgrTest::AddContactL()
	{
	// Create the contact
	CContactItem* cntItem = iCntItemBldr->GetCntItemLC();    

    TInt cntID = iPersistLayer->PersistenceBroker().CreateL(*cntItem, 0);

	CleanupStack::PopAndDestroy(cntItem);
	cntItem = NULL;
	return cntID;
	}


TInt CPlTransMgrTest::AddAndCommitContactL()
	{
	iPersistLayer->TransactionManager().StartTransactionL();
	TInt cntID = AddContactL();
	iPersistLayer->TransactionManager().CommitCurrentTransactionL(0);
	return cntID;
	}


void CPlTransMgrTest::DeleteContactL(TInt aCntID) 
	{
    CContactItem* cntItem = iPersistLayer->PersistenceBroker().DeleteLC(aCntID, 0, ESendEvent);
	CleanupStack::PopAndDestroy(cntItem);
	}


void CPlTransMgrTest::UpdateContactL(TInt aCntID)
	{
	CContactItem* cntItem = iPersistLayer->PersistenceBroker().ReadLC(aCntID, *iMatchAll, EPlAllInfo, 0);
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

	iPersistLayer->PersistenceBroker().UpdateL(*cntItem, 0);
	CleanupStack::PopAndDestroy(cntItem);
	}



TBool CPlTransMgrTest::CheckChangedFieldL(TInt aCntID)
	{
	CContactItem* cntItem = iPersistLayer->PersistenceBroker().ReadLC(aCntID, *iMatchAll, EPlAllInfo, 0);
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



TBool CPlTransMgrTest::IsContactInDbL(TInt aCntID)
	{
	CContactItem* cntItem = NULL;
	TRAPD(err, cntItem = iPersistLayer->PersistenceBroker().ReadLC(aCntID, *iMatchAll, EPlAllInfo, 0);
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

@SYMTestCaseID     PIM-T-PLTRANSACTIONMANAGER-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L("@SYMTESTCaseID:PIM-T-PLTRANSACTIONMANAGER-0001 ----------- PLTransactionManager tests ----------"));


	CPlTransMgrTest* tranMgrTest = CPlTransMgrTest::NewLC();	

	tranMgrTest->RunTestsL();

	test.End();
    CleanupStack::PopAndDestroy(2); // test.Close & tranMgrTest
	}

LOCAL_C void CleanupFilesL()
	{
    // delete the database file
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect() );
	CPersistenceLayer* persistLayer = CPersistenceLayer::NewLC(fs);
    MLplContactsFile& cntFile = persistLayer->ContactsFileL();
    if (cntFile.DatabaseExistsL(nsPlTransMgr::KFilename() ) )
    	{
    	cntFile.DeleteL(nsPlTransMgr::KFilename() );
    	}
    CleanupStack::PopAndDestroy(2, &fs); // and persistLayer
	}


GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        return KErrNoMemory;
          
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;

    TRAP_IGNORE(CleanupFilesL() );

    test(__NB_failures == 0);

    // Cleanup
	delete activeScheduler;
    delete cleanupStack;
	return err;
    }
