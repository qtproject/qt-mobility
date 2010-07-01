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


#include "testimpoom.h"

CTestImpOOM::~CTestImpOOM()
/**
 * Destructor
 */
	{
	}

CTestImpOOM::CTestImpOOM()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImpOOM);
	}

TVerdict CTestImpOOM::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards in OOM Test"));
	
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestImpOOM::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EPass);
	TInt tryCount = 1;
	TInt err = KErrNone;
	TBool success = EFalse;
	CArrayPtr<CContactItem>* item = NULL;

	CContactDatabase *dBase = NULL;
   	_LIT(KTestDbName, "c:importcontact.cdb");
   	RFs fsSession;
	RFileReadStream readStream;
	
	//replace existing database
	dBase = CContactDatabase::ReplaceL(KTestDbName);
	CleanupStack::PushL(dBase);
	
	//connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName processFileName = RProcess().FileName();
	TParsePtrC parse(processFileName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(KImportOOM());
		
	//Opens a file containing a stream and prepares the stream for reading
	User::LeaveIfError(readStream.Open(fsSession, fileName, EFileRead));
    CleanupClosePushL(readStream);

	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		TRAP(err, item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EDefault));
		 
		if ( err == KErrNone )
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("OOM testing of CContactDatabase::ImportContactsL Api is done"));
			break;
			}
		if ( err != KErrNoMemory )
			{
			SetTestStepResult(EFail);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		tryCount++;
		}
	 
	// item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EDefault);
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	TInt contactsCount = (*item).Count();
	INFO_PRINTF2(_L("Total number of contacts imported is %d "), contactsCount);
	if (contactsCount == 0)
		{
		SetTestStepResult(EFail);
		}

	readStream.Close();
	
	// Cleanup
	item->ResetAndDestroy();
	delete item;
	CleanupStack::PopAndDestroy(&readStream);
	fsSession.Close();
    CleanupStack::PopAndDestroy(&fsSession);
    CleanupStack::PopAndDestroy(dBase);

	return TestStepResult();
	}

TVerdict CTestImpOOM::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	INFO_PRINTF1(_L("Completed Import of vCards in OOM Test"));
	return TestStepResult();
	}
	
