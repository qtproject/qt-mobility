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
* Persistence Layer Performance Test module
* These tests compare the Contact Persistence Layer with the original
* Contacts model.
* This class is an implementation of the bridge pattern. It represents
* the left hand side of the pattern or an abstraction class. The decoupled
* implementation classes can be found in T_CntTestImplentor.h
*
*/


#include <e32test.h>
#include <coreappstest/testserver.h>
#include <cntitem.h>
#include <cntfldst.h>

_LIT(KTestName, "T_PlPerformance"); // Used by Testhelper - gives an warning under ARMv5

#include "t_plperformance.h"

#include "testhelpers.h"
#include "persistencelayer.h"

#include "t_cnttestimpldefs.h"


//#define RUN_CNT
#define RUN_PL
//#define RUN_CLIENT_SERVER




// Implementation of CPlPerformance member functions

CPlPerformance* CPlPerformance::NewLC (CCntTestImplementor& aCntTestImpl, const TDesC& aFilename)
	{
	RDebug::Print(_L("CPlPerformance::NewLC"));
		
	CPlPerformance* self = new (ELeave) CPlPerformance(aCntTestImpl);
	CleanupStack::PushL(self);

	self->ConstructL(aFilename);
	return self;
	}


void CPlPerformance::ConstructL(const TDesC& aFilename)
	{
	RDebug::Print(_L("CPlPerformance::ConstructL"));
		
	// 0
	iCntTestImpl.CreateDatabaseL(nsPlPerformance::KCreatePlPerform, ETrue);
	// 1
	iCntItemBldr = CCntItemBuilder::NewLC(iCntTestImpl.GetSysTemplate());
	CleanupStack::Pop(iCntItemBldr);
	// 2
	iIdBuffer = new (ELeave) CIdBuffer();
	// 3
	iMatchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
										   CContactItemViewDef::EIncludeHiddenFields);
	iMatchAll->AddL(KUidContactFieldMatchAll);
	CleanupStack::Pop(iMatchAll);
	// 4
	iCsvWriter = CCsvWriter::NewLC(aFilename);
	CleanupStack::Pop(iCsvWriter);
	}



CPlPerformance::~CPlPerformance()
	{
	RDebug::Print(_L("~CPlPerformance()"));
		
	// 4
	delete iCsvWriter;
	// 3	
	delete iMatchAll;
	// 2
	delete iIdBuffer;
	// 1
	delete iCntItemBldr;
	// 0
	iCntTestImpl.CloseDatabase();
	}


void CPlPerformance::CreateContactsBatchL(TInt aSize)
	{
	test.Next(_L(" ->... Create"));
	RDebug::Print(_L("CreateContactsTestL(TInt aSize)"));		
				
	TTime startTime;
	startTime.UniversalTime();
	
	for (int ii = 0; ii < aSize; ii+=nsPlPerformance::KBatchSize)
		{
		CreateContactsTestL(aSize<nsPlPerformance::KBatchSize ?
							aSize:nsPlPerformance::KBatchSize);
		if (ii >= nsPlPerformance::KBatchSize - 1)
			{
			iCntTestImpl.CompactL();		
			}
		}	

	TTime finishTime;			
	finishTime.UniversalTime();
	// Temporary required for ARMv5, adress of Int64() is taken in
	// parameter to Format method otherwise.
	TReal lapsed = (TReal) finishTime.MicroSecondsFrom(startTime).Int64();
	
	TBuf8<64> row;
	// _LIT8 gives an error - illegal implicit conversion from long 
	row.Format(_L8("Create,%d,%.4f\r\n"), aSize, lapsed/1000000);
	iCsvWriter->WriteNextLineL(row);
	
	/*TBool compressFlag = */iCntTestImpl.CompressRequired();	
	
	}


void CPlPerformance::CreateContactsTestL(TInt aSize)
	{
	CContactItem* tempCntItem = NULL;
	// create a contact & add to the contact db.
	for (int ii = 0; ii < aSize; ++ii)
		{
		tempCntItem = iCntItemBldr->GetCntItemLC();
    	iIdBuffer->AddL(iCntTestImpl.CreateL(*tempCntItem));
		CleanupStack::PopAndDestroy(tempCntItem);
		tempCntItem = NULL;
		}
	}

void CPlPerformance::UpdateContactsBatchL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L(" ->... Update"));
	RDebug::Print(_L("CPlPerformance::UpdateContactsTestL"));
		
	ShuffleContactIdsL();

	TInt size = iIdBuffer->Count() < aEndPos ? iIdBuffer->Count() : aEndPos;
	
	TTime startTime;	
	startTime.UniversalTime();

	for (int ii = aStartPos; ii < aEndPos; ii+=nsPlPerformance::KBatchSize)
		{
		UpdateContactsTestL(ii, size < nsPlPerformance::KBatchSize + ii ?
		 					    size : nsPlPerformance::KBatchSize + ii);
		if (ii >= nsPlPerformance::KBatchSize - 1)
			{
			iCntTestImpl.CompactL();		
			}
		}

	TTime finishTime;
	finishTime.UniversalTime();
	TReal lapsed = (TReal) finishTime.MicroSecondsFrom(startTime).Int64();
			
	TBuf8<84> row;
	// _LIT8 gives an error - illegal implicit conversion from long 
	row.Format(_L8("Update,%d,%.4f\r\n"), size - aStartPos, lapsed/1000000);
	iCsvWriter->WriteNextLineL(row);

	}

void CPlPerformance::UpdateContactsTestL(const TInt aStartPos, const TInt aEndPos)
	{
	CContactItem* tempCntItem = NULL;
	
	for (int ii = aStartPos; ii < aEndPos; ++ii)//size; ++ii)
		{
		// Modify the data in a contact from the db
		tempCntItem = iCntTestImpl.OpenLC(iIdBuffer->Get(ii), *iMatchAll);
		TCnt cnt(tempCntItem);
		cnt[KUidContactFieldGivenName] = nsPlPerformance::KName;
		cnt[KUidContactFieldPhoneNumber] & KWork & KVoice = nsPlPerformance::KNumber;
		cnt[KUidContactFieldPhoneNumber] & KWork & KVoice & KCell += nsPlPerformance::KNumber;
		cnt[KUidContactFieldEMail] & KWork = nsPlPerformance::KEmail;
		cnt[KUidContactFieldAddress] & KHome = nsPlPerformance::KHomeAdd;
		iCntTestImpl.UpdateL(*tempCntItem);
		CleanupStack::PopAndDestroy(tempCntItem);
		}
	}


void CPlPerformance::ReadContactsTestL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L(" ->... Read"));
	RDebug::Print(_L("ReadContactsTestL"));
		
	ShuffleContactIdsL();

	TTime startTime;
	startTime.UniversalTime();

	TInt size = iIdBuffer->Count() < aEndPos ? iIdBuffer->Count() : aEndPos;
	for (int ii = aStartPos; ii < size; ++ii)
		{
		CleanupStack::PopAndDestroy(iCntTestImpl.ReadLC(iIdBuffer->Get(ii), *iMatchAll));
		}
	TTime finishTime;
	
	finishTime.UniversalTime();
	TReal lapsed = (TReal) finishTime.MicroSecondsFrom(startTime).Int64();			
	
	TBuf8<64> row;
	// _LIT8 gives an error - illegal implicit conversion from long 
	row.Format(_L8("Read,%d,%.4f\r\n"), size - aStartPos, lapsed/1000000);
	iCsvWriter->WriteNextLineL(row);
	}



void CPlPerformance::DeleteContactsBatchL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L(" ->.. Delete."));
	RDebug::Print(_L("DeleteContactsTestL"));

	ShuffleContactIdsL();

	TInt size = iIdBuffer->Count() < aEndPos ? iIdBuffer->Count() : aEndPos;					
	TTime startTime;
	startTime.UniversalTime();
	
	for (int ii = aStartPos; ii < aEndPos; ii+=nsPlPerformance::KBatchSize)
		{
		DeleteContactsTestL(ii, size < nsPlPerformance::KBatchSize + ii ?
		 					    size : nsPlPerformance::KBatchSize + ii);
		if (ii >= nsPlPerformance::KBatchSize - 1)
			{
			iCntTestImpl.CompactL();		
			}
		}			

	TTime finishTime;
	finishTime.UniversalTime();
	TReal lapsed = (TReal) finishTime.MicroSecondsFrom(startTime).Int64();
			
	TBuf8<64> row;
	// _LIT8 gives an error - illegal implicit conversion from long 
	row.Format(_L8("Delete,%d,%.4f\r\n"), size - aStartPos, lapsed/1000000);
	iCsvWriter->WriteNextLineL(row);
	
	/*TBool compressFlag = */iCntTestImpl.CompressRequired();	
	}



void CPlPerformance::DeleteContactsTestL(const TInt aStartPos, const TInt aEndPos)
	{
	for (int ii = aStartPos; ii < aEndPos; ++ii)
		{
		iCntTestImpl.DeleteL(iIdBuffer->Get(ii));
		}
	}


void CPlPerformance::ShuffleContactIdsL()
	{
	RDebug::Print(_L("ShuffleContactIdsL"));
		
	if (iShuffle) // only ever shuffle once
		{
		iIdBuffer->RandomShuffleL();
		iShuffle = false;	
		}
	}

// End of CPlPerformance member variables


// CIdBuffer Class
CIdBuffer::CIdBuffer()
	{
	RDebug::Print(_L("CIdBuffer()"));
		
	iContactIds = new CArrayFixFlat<TInt>(nsPlPerformance::KSize1Sample 
					+ nsPlPerformance::KSize2Sample	+ nsPlPerformance::KSize3Sample 
					+ nsPlPerformance::KSize4Sample + nsPlPerformance::KSize5Sample 
					+ nsPlPerformance::KSize6Sample + 1);
	if (!iContactIds)
		{
		test(EFalse);
		}
	}

void CIdBuffer::AddL(TInt aID)
	{
	test.Printf(_L("Add Contact ID:%d \n"),aID);
	iContactIds->InsertL(iContactIds->Count(), aID);	
	}

void CIdBuffer::RandomShuffleL()
	{
	RDebug::Print(_L("CIdBuffer::RandomShuffleL"));
		
	TInt upperBounds  = iContactIds->Count();
	TInt orgVal  = 0;
	TInt newVal  = 0;
	TInt ranPos  = 0;
		
	for (TInt ii = 0; ii < upperBounds; ++ii)
		{
		ranPos = Math::Rand(nsPlPerformance::KSeed) % upperBounds;
		orgVal  = iContactIds->At(ii);  	  		
		newVal  = iContactIds->At(ranPos);
		iContactIds->Delete(ranPos);
		iContactIds->InsertL(ranPos, orgVal);  	
		iContactIds->Delete(ii);
		iContactIds->InsertL(ii, newVal); 
		}
	}

TInt CIdBuffer::Get(TInt& aIndex) const
	{	
	test.Printf(_L("Get Contact ID @ index: %d \n"),aIndex);
	return iContactIds->At(aIndex);
	}

TInt CIdBuffer::Count() const
	{
	RDebug::Print(_L("CIdBuffer::Count()"));
		
	return iContactIds->Count();
	}
// End of CIdBuffer class implementation






// CCsvWriter  class 
CCsvWriter::~CCsvWriter()
	{
	RDebug::Print(_L("~CCsvWriter()"));
		
	iSrcFile.Close();
	}

CCsvWriter* CCsvWriter::NewLC(const TDesC& filename)
	{
	RDebug::Print(_L("CCsvWriter::NewLC"));
		
	CCsvWriter* self = new (ELeave) CCsvWriter();
	CleanupStack::PushL (self);
	User::LeaveIfError (self->iFs.Connect());
	
	TInt err = self->iSrcFile.Open (self->iFs, filename, EFileWrite);
	if (err == KErrNotFound)
		err  = self->iSrcFile.Create(self->iFs, filename, EFileWrite);
	if (err < KErrNone)
		User::Leave(err);
	
	self->ConstructL();
	return self;
	}

void CCsvWriter::ConstructL()
	{
	RDebug::Print(_L("ConstructL()"));
		
	// Write the header line.
	WriteNextLineL(nsPlPerformance::KColumnHeader);
	}

void CCsvWriter::WriteNextLineL(const TDesC8& aCsvLine)
	{
	RDebug::Print(_L("WriteNextLineL"));
		
	User::LeaveIfError(iSrcFile.Write(aCsvLine));
	}
	
// End of CCsvWriter class implementation

// Temporary CompressRequired Test
void DoPlCompressRequiredTestsL(CPlPerformance& aCntTests)
    {
    RDebug::Print(_L("DoPlCompressRequiredTestsL"));
   	test.Next(_L(" -> Adding contacts..."));
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize1Sample);
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry,  
								  nsPlPerformance::KSize1Sample);	
	
								  
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize2Sample);
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample);
								  	
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize3Sample);  
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample+
								  nsPlPerformance::KSize3Sample);	
    }


void DoPlTestsL(CPlPerformance& aCntTests)
    {
	RDebug::Print(_L("DoPlTestsL"));
		
   	test.Next(_L(" -> Adding contacts..."));
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize1Sample);
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize2Sample);
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize3Sample);
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize4Sample);
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize5Sample);
	aCntTests.CreateContactsBatchL(nsPlPerformance::KSize6Sample);


   	test.Next(_L(" -> Updating contacts..."));
	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry,  
								  nsPlPerformance::KSize1Sample);
								  
	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample);
								  
	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample+
								  nsPlPerformance::KSize3Sample);

	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample);

	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample);
								  
	aCntTests.UpdateContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample +
								  nsPlPerformance::KSize6Sample);
								  
   	test.Next(_L(" -> Reading contacts..."));
	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry,  
								  nsPlPerformance::KSize1Sample);
								  
	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample);
								  
	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample+
								  nsPlPerformance::KSize3Sample);
								  
	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample);

	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample);
								  
	aCntTests.ReadContactsTestL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample +
								  nsPlPerformance::KSize6Sample);

   	test.Next(_L(" -> Deleting contacts..."));
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry,  
								  nsPlPerformance::KSize1Sample);
								  
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample);
								  
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample+
								  nsPlPerformance::KSize3Sample);

	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample);

	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample);
								  
	aCntTests.DeleteContactsBatchL(nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample,
								  nsPlPerformance::KFirstEntry  + 
								  nsPlPerformance::KSize1Sample +
								  nsPlPerformance::KSize2Sample +
								  nsPlPerformance::KSize3Sample +
								  nsPlPerformance::KSize4Sample +
								  nsPlPerformance::KSize5Sample +
								  nsPlPerformance::KSize6Sample);
	}

/**

@SYMTestCaseID PIM-T-PIPERFORMANCE-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L ("@SYMTestCaseID:PIM-T-PIPERFORMANCE-0001 ----------- Performance tests ---------- "));

	// Setup test
	CPlPerformance* performTest = NULL;
	
	// Run the old contact model test
#ifdef RUN_CNT
	test.Start(_L(" ---- Contact Model Tests ---- "));

   	__UHEAP_MARK;

	CCntTestImpl* oldCntTestImpl = CCntTestImpl::NewLC();
	performTest = CPlPerformance::NewLC(*oldCntTestImpl, 
						nsPlPerformance::KContactCSV);

	DoPlTestsL(*performTest);
	
	CleanupStack::PopAndDestroy(performTest);
	performTest	   = NULL;
	CleanupStack::PopAndDestroy(oldCntTestImpl);
	oldCntTestImpl = NULL;
	
	__UHEAP_MARKEND;
	test.End();

#endif // RUN_CNT
	
	// Run the new persistence layer test
#ifdef RUN_PL
	test.Start(_L(" ---- Persistence Layer Tests ---- "));

	__UHEAP_MARK;
	
	CPlTestImpl* plCntTestImpl = CPlTestImpl::NewLC();
  	performTest = CPlPerformance::NewLC(*plCntTestImpl, 
  						nsPlPerformance::KPersistCSV);
	
	DoPlTestsL(*performTest);
	//DoPlCompressRequiredTestsL(*performTest);
	
	CleanupStack::PopAndDestroy(performTest);
	performTest	   = NULL;
	CleanupStack::PopAndDestroy(plCntTestImpl);
	plCntTestImpl  = NULL;
	
	__UHEAP_MARKEND;  
	test.End();
  
#endif // RUN_PL 



	// Run the new client server test
#ifdef RUN_CLIENT_SERVER
	test.Start(_L(" ---- Client Server Tests ---- "));

	__UHEAP_MARK;
	
	CClientSvrImpl* clsvrTestImpl = CClientSvrImpl::NewLC();
  	performTest = CPlPerformance::NewLC(*clsvrTestImpl, 
  						nsPlPerformance::KClSvrCSV);
	
	DoPlTestsL(*performTest);
	
	CleanupStack::PopAndDestroy(performTest);
	performTest	   = NULL;
	CleanupStack::PopAndDestroy(clsvrTestImpl);
	clsvrTestImpl  = NULL;
	
	__UHEAP_MARKEND;  
	test.End();
  
#endif // RUN_CLIENT_SERVER 


	test.End();
    CleanupStack::PopAndDestroy(1); // test.Close
	}




GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
  	if (!cleanupStack)
      return KErrNoMemory;
        
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        return KErrNoMemory;
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;

	test(__NB_failures == 0);
		
    // Cleanup
    delete activeScheduler;
 	delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }





