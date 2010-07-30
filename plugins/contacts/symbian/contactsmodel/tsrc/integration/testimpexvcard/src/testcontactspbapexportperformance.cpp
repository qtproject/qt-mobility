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

 
#include "testcontactspbapexportperformance.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

CTestContactsPBAPExportPerformance::~CTestContactsPBAPExportPerformance()
/**
 * Destructor
 */
	{
	}

CTestContactsPBAPExportPerformance::CTestContactsPBAPExportPerformance()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestContactsPBAPExportPerformance);
	}

TVerdict CTestContactsPBAPExportPerformance::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Performance test"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestContactsPBAPExportPerformance::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	TestPerformanceL();
	return TestStepResult();
	}
	
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CTestContactsPBAPExportPerformance::doTestStepPostambleL()
 	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	
	INFO_PRINTF1(_L("Completed Performance test "));
	return TestStepResult();
	}

void CTestContactsPBAPExportPerformance::TestPerformanceL()
	{
	// Retreive the file name to be exported
   	RFs fsSession;
	RFileWriteStream writeStream;
	CleanupClosePushL(writeStream);
	
	// connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	TPtrC exportTo;
	GetStringFromConfig(ConfigSection(), KExportTo, exportTo);
		
   	// Makes one or more directories.
   	fsSession.MkDirAll(exportTo);
   	
	// Replaces a single file with another
	User::LeaveIfError(writeStream.Replace(fsSession, exportTo, EFileWrite));
		
	INFO_PRINTF1(_L("Exporting Contact....."));

	// Existing database
   	TPtrC databaseFile(_L("C:contactDb.cdb"));
	
	CContactDatabase* dBase = NULL;
	CContactIdArray* idArray = NULL;
   	
	// Open existing database
	dBase = CContactDatabase::OpenL(databaseFile);
	CleanupStack::PushL(dBase);
	
	CCntFilter* exportFilter = CCntFilter::NewL();
	CleanupStack::PushL(exportFilter);
	exportFilter->SetContactFilterTypeCard(ETrue);
	dBase->FilterDatabaseL(*exportFilter);
	idArray = exportFilter->iIds;	
		
	CleanupStack::PushL(idArray);
	
	TPtrC standard;
	GetStringFromConfig(ConfigSection(), KStandard, standard);
		
	if(standard.Compare(KOldApi) == 0)
		{
		// Take the base time taken, using the existing/old API
		ExportWithOldApiL(dBase, idArray, writeStream);
		}
	else if(standard.Compare(KV21) == 0)
		{
		// Take the time taken by the new PBAP export API for 2.1 export
		ExportWithPBAPVersionL(dBase, KV21, idArray, writeStream);
		}
	
	else if(standard.Compare(KV30) == 0)
		{
		// Take the time taken by the new PBAP export API for 3.0 export
		ExportWithPBAPVersionL(dBase, KV30, idArray, writeStream);
		}
		
	INFO_PRINTF1(_L("Exported Contact"));
	
	INFO_PRINTF2(_L("Total number of contacts in database %d "), dBase->CountL());
	
	// Cleanup
    CleanupStack::Pop(idArray);
    CleanupStack::PopAndDestroy(exportFilter);

    CleanupStack::PopAndDestroy(dBase);
    CleanupStack::PopAndDestroy(&fsSession);
    CleanupStack::PopAndDestroy(&writeStream);
	}

/** Exports a vCard with old Api(without PBAP support) to note the time taken by the Api 
@param	aDBase The database to export from
@param	aIdArray The contact item ID array to export from database
@param  aWriteStream The write stream to write to
*/	
void CTestContactsPBAPExportPerformance::ExportWithOldApiL(CContactDatabase* aDBase, CContactIdArray* aIdArray, RFileWriteStream& aWriteStream)
	{
	TTimeIntervalMicroSeconds timeTaken;
	TInt64 baseTime;
	NOW(startTime);
	aDBase->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *aIdArray, aWriteStream, 0);
	NOW(endTime);
	
	aWriteStream.CommitL();
	aWriteStream.Close();
	timeTaken = endTime.MicroSecondsFrom(startTime);
		
	baseTime = timeTaken.Int64();
	
	_LIT(KFormat,"%Li");	 
	TBuf<KBufLength> buf;
	buf.Format(KFormat, baseTime);
	TPtrC ptr(buf);
	INFO_PRINTF2(_L("Base Time taken: %Li"), baseTime);
	WriteStringToConfig(ConfigSection(), KBaseTime, ptr); 
	}

/** Exports a vCard with the specified version(with PBAP support) to note the time taken by the Api 
@param	aDBase The database to export from
@param  aVersion The PBAP version to export 
@param	aIdArray The contact item ID array to export from database
@param  aWriteStream The write stream to write to
*/	
void CTestContactsPBAPExportPerformance::ExportWithPBAPVersionL(CContactDatabase* aDBase, const TDesC& aVersion, CContactIdArray* aIdArray, RFileWriteStream& aWriteStream)
	{
	TTimeIntervalMicroSeconds timeTaken;
	TInt64 finalTime;
	TInt64 baseTime;
	TPtrC baseValue;
	
	TVCardVersion version = EPBAPVCard21;
	
	if(aVersion.Compare(KVersion30) == 0)
		{
		version = EPBAPVCard30;	
		}
	
	TInt64 contactFilter = 0;
	
	NOW(startTime);
	aDBase->ExportSelectedContactsL(TUid::Uid(KUidPBAPVCardConvImpl), *aIdArray, aWriteStream, 0, contactFilter, NULL, version ,1);
	NOW(endTime);
	aWriteStream.CommitL();
	aWriteStream.Close();
	timeTaken = endTime.MicroSecondsFrom(startTime);
	finalTime = timeTaken.Int64();
		
	GetStringFromConfig(KExportvCardPerformanceOld, KBaseTime, baseValue);
	TLex lexBuf(baseValue);
	lexBuf.Val(baseTime);
	
	INFO_PRINTF2(_L("Time taken: %Li"), finalTime);
	
	INFO_PRINTF2(_L("Time diff b/w base and final: %Li"), finalTime-baseTime);
		
	if(finalTime > (baseTime+(KTolerance*baseTime)))
		{
		ERR_PRINTF1(_L("Time taken has exceeded the expected limit"));
		SetTestStepResult(EFail);		
		}
	else
		{
		INFO_PRINTF1(_L("Time taken is lower than expected limit"));
		}
	
	}
