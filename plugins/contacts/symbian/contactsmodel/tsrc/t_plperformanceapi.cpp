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

_LIT(KTestName, "T_PlPerformanceAPI"); // Used by Testhelper - gives an warning under ARMv5
_LIT(KTestContactFileName, "C:PlPerform.cdb");

#include "t_plperformanceapi.h"

#include "testhelpers.h"
#include "persistencelayer.h"

#include "t_cnttestimpldefs.h"


//#define RUN_CNT
//#define RUN_PL
#define RUN_CLIENT_SERVER




// Implementation of CPlPerformanceAPI member functions

CPlPerformanceAPI* CPlPerformanceAPI::NewLC (CCntTestImplementor& aCntTestImpl, const TDesC& aFilename)
	{
	RDebug::Print(_L("CPlPerformanceAPI::NewLC"));
		
	CPlPerformanceAPI* self = new (ELeave) CPlPerformanceAPI(aCntTestImpl);
	CleanupStack::PushL(self);

	self->ConstructL(aFilename);
	return self;
	}


void CPlPerformanceAPI::ConstructL(const TDesC& aFilename)
	{
	RDebug::Print(_L("CPlPerformanceAPI::ConstructL"));
		
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



CPlPerformanceAPI::~CPlPerformanceAPI()
	{
	RDebug::Print(_L("~CPlPerformanceAPI()"));
		
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

void CPlPerformanceAPI::CreateContactsBatchL(TInt aSize)
	{
	test.Next(_L("->... Create"));

	RDebug::Print(_L("CreateContactsTestL(TInt aSize)"));
	TBool testFlag = EFalse;
	testFlag = iCntTestImpl.DefaultContactDatabaseExistsL();
	test.Printf(_L("CreateContactsBatchL_1, DefaultContactDatabaseExistsL:%d \n"),testFlag);
	
				
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
	
	// API Tests
	testFlag = iCntTestImpl.CompressRequired();
	test.Printf(_L("CreateContactsBatchL, CompressRequired:%d \n"),testFlag);	
	
	testFlag = iCntTestImpl.IsDamaged();
	test.Printf(_L("CreateContactsBatchL, IsDamaged:%d \n"),testFlag);
	
	testFlag = iCntTestImpl.DefaultContactDatabaseExistsL();
	test.Printf(_L("CreateContactsBatchL, DefaultContactDatabaseExistsL:%d \n"),testFlag);
		
	testFlag = iCntTestImpl.ContactDatabaseExistsL(KTestContactFileName);
	test.Printf(_L("CreateContactsBatchL, ContactDatabaseExistsL:%d \n"),testFlag);
	
	TInt theCount;
	theCount = iCntTestImpl.FileSize();
	test.Printf(_L("CreateContactsBatchL, FileSize:%d \n"),theCount);
	
	theCount = iCntTestImpl.WastedSpaceInBytes();
	test.Printf(_L("CreateContactsBatchL, WastedSpaceInBytes:%d \n"),theCount);	
	
	TBuf<190> defaultName;
	iCntTestImpl.GetDefaultNameL(defaultName);
	test.Printf(_L("CreateContactsBatchL, GetDefaultNameL: %s \n"),defaultName.PtrZ());	
	defaultName.Delete(0,defaultName.Size());
	
	TDriveUnit driveUnit;
	iCntTestImpl.DatabaseDrive(driveUnit);
	TBuf<190> driveName = driveUnit.Name();
	test.Printf(_L("CreateContactsBatchL, DatabaseDrive: %s \n"),driveName.PtrZ());		

	}


void CPlPerformanceAPI::CreateContactsTestL(TInt aSize)
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

void CPlPerformanceAPI::UpdateContactsBatchL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L("->... Update"));

	RDebug::Print(_L("CPlPerformanceAPI::UpdateContactsTestL"));
		
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

void CPlPerformanceAPI::UpdateContactsTestL(const TInt aStartPos, const TInt aEndPos)
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


void CPlPerformanceAPI::ReadContactsTestL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L("->... Read"));

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


void CPlPerformanceAPI::DeleteContactsBatchL(const TInt aStartPos, const TInt aEndPos)
	{
	test.Next(_L("->.. Delete."));

	RDebug::Print(_L("DeleteContactsTestL"));
	TBool testFlag = EFalse;

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
	
	// API Tests
	testFlag = iCntTestImpl.CompressRequired();
	test.Printf(_L("DeleteContactsBatchL, CompressRequired:%d \n"),testFlag);	
	
	testFlag = iCntTestImpl.IsDamaged();
	test.Printf(_L("DeleteContactsBatchL, IsDamaged:%d \n"),testFlag);
	
	testFlag = iCntTestImpl.DefaultContactDatabaseExistsL();
	test.Printf(_L("DeleteContactsBatchL, DefaultContactDatabaseExistsL:%d \n"),testFlag);	
	
	testFlag = iCntTestImpl.ContactDatabaseExistsL(KTestContactFileName);
	test.Printf(_L("DeleteContactsBatchL, ContactDatabaseExistsL:%d \n"),testFlag);
	
	TInt theCount;
	theCount = iCntTestImpl.FileSize();
	test.Printf(_L("DeleteContactsBatchL, FileSize:%d \n"),theCount);
	
	theCount = iCntTestImpl.WastedSpaceInBytes();
	test.Printf(_L("DeleteContactsBatchL, WastedSpaceInBytes:%d \n"),theCount);			
	
	}



void CPlPerformanceAPI::DeleteContactsTestL(const TInt aStartPos, const TInt aEndPos)
	{
	for (int ii = aStartPos; ii < aEndPos; ++ii)
		{
		iCntTestImpl.DeleteL(iIdBuffer->Get(ii));
		}
	}
	
void CPlPerformanceAPI::DeleteDefaultDatabaseTestL()
	{
	TBool testFlag = iCntTestImpl.DefaultContactDatabaseExistsL();
	test.Printf(_L("Check if Default Database exists before deleting"));
	test.Printf(_L("DeleteDefaultDatabaseTestL, DefaultContactDatabaseExistsL:%d \n"),testFlag);
		
	test.Printf(_L("Delete Default Database"));		
	iCntTestImpl.CloseDatabase();
	iCntTestImpl.DeleteDefaultFileL();
	test.Printf(_L("DeleteDefaultDatabaseTestL, DeleteDefaultFileL"));
		
	testFlag = iCntTestImpl.DefaultContactDatabaseExistsL();
	test.Printf(_L("Check if File was deleted"));
	test.Printf(_L("DeleteDefaultDatabaseTestL, DefaultContactDatabaseExistsL:%d \n"),testFlag);
	}
	
void CPlPerformanceAPI::ListDatabaseTestL()
	{
	TDriveUnit driveUnit = _L("c:");
	TBuf<190> driveName;
	CDesCArray* dataBaseList = iCntTestImpl.ListDatabasesL(driveUnit);
	CleanupStack::PushL(dataBaseList);
	TInt theCount = dataBaseList->Count();
	for(TInt i = 0; i< theCount; i++)
		{
		driveName = (*dataBaseList)[i];
		test.Printf(_L("ListDatabaseTestL, ListDatabasesL: %s \n"),driveName.PtrZ());
//		driveName.Delete(0,driveName.Size());
		}
//	dataBaseList->Reset();
	CleanupStack::PopAndDestroy(dataBaseList);
	}	

void CPlPerformanceAPI::ShuffleContactIdsL()
	{
	RDebug::Print(_L("ShuffleContactIdsL"));
		
	if (iShuffle) // only ever shuffle once
		{
		iIdBuffer->RandomShuffleL();
		iShuffle = false;	
		}
	}

// End of CPlPerformanceAPI member variables


// CIdBuffer Class
CIdBuffer::CIdBuffer()
	{
	RDebug::Print(_L("CIdBuffer()"));
		
	iContactIds = new(ELeave) CArrayFixFlat<TInt>(nsPlPerformance::KSize1Sample 
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

void DoPlCompressRequiredTestsL(CPlPerformanceAPI& aCntTests)
    {
    RDebug::Print(_L("DoPlCompressRequiredTestsL"));
   	test.Next(_L("-> Adding contacts..."));

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

	aCntTests.DeleteDefaultDatabaseTestL();
//	aCntTests.ListDatabaseTestL();
   }

/**

@SYMTestCaseID     PIM-T-PLPERFORMANCEAPI-0001

*/

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L("@SYMTESTCaseID:PIM-T-PLPERFORMANCEAPI-0001 ----------- Performance tests ----------"));


	// Setup test
	CPlPerformanceAPI* performTest = NULL;
	
	// Run the old contact model test
#ifdef RUN_CNT
	test.Start(_L("---- Contact Model Tests ----"));


   	__UHEAP_MARK;

	CCntTestImpl* oldCntTestImpl = CCntTestImpl::NewLC();
	performTest = CPlPerformanceAPI::NewLC(*oldCntTestImpl, 
						nsPlPerformance::KContactCSV);

	//DoPlTestsL(*performTest);
	DoPlCompressRequiredTestsL(*performTest);
	
	CleanupStack::PopAndDestroy(performTest);
	performTest	   = NULL;
	CleanupStack::PopAndDestroy(oldCntTestImpl);
	oldCntTestImpl = NULL;
	
	__UHEAP_MARKEND;
	test.End();

#endif // RUN_CNT
	
	// Run the new persistence layer test
#ifdef RUN_PL
	test.Start(_L("---- Persistence Layer Tests ----"));


	__UHEAP_MARK;
	
	CPlTestImpl* plCntTestImpl = CPlTestImpl::NewLC();
  	performTest = CPlPerformanceAPI::NewLC(*plCntTestImpl, 
  						nsPlPerformance::KPersistCSV);
	
	//DoPlTestsL(*performTest);
	DoPlCompressRequiredTestsL(*performTest);
	
	CleanupStack::PopAndDestroy(performTest);
	performTest	   = NULL;
	CleanupStack::PopAndDestroy(plCntTestImpl);
	plCntTestImpl  = NULL;
	
	__UHEAP_MARKEND;  
	test.End();
  
#endif // RUN_PL 



	// Run the new client server test
#ifdef RUN_CLIENT_SERVER
	test.Start(_L("---- Client Server Tests ----"));


	__UHEAP_MARK;
	
	CClientSvrImpl* clsvrTestImpl = CClientSvrImpl::NewLC();
  	performTest = CPlPerformanceAPI::NewLC(*clsvrTestImpl, 
  						nsPlPerformance::KClSvrCSV);
	
	// DoPlTestsL(*performTest);
	DoPlCompressRequiredTestsL(*performTest);
	
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





