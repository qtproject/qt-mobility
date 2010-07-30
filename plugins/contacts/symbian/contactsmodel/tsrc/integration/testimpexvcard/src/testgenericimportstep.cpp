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

 
#include "testgenericimportstep.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#endif

CTestGenericImportStep::~CTestGenericImportStep()
/**
 * Destructor
 */
	{
	}

CTestGenericImportStep::CTestGenericImportStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestGenericImportStep);
	}

TVerdict CTestGenericImportStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start Import of vCards"));
	iScheduler = new (ELeave) CActiveScheduler;

	CActiveScheduler::Install(iScheduler);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestGenericImportStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	ImportContactsL();
	return TestStepResult();
	}

TVerdict CTestGenericImportStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	iScheduler = NULL;
	INFO_PRINTF1(_L("Completed Import of vCards "));
	return TestStepResult();
	}

void CTestGenericImportStep::ImportContactsL()
	{
	// Retreive the file name to be Imported
   	RFs fsSession;
	RFileReadStream readStream;
	CleanupClosePushL(readStream);
	
	// connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	GetInputFromIni();
	
	// Open the the file from which the contact data has to be imported
   	User::LeaveIfError(readStream.Open(fsSession, iImportFile, EFileRead));
   	
   	CContactDatabase *dBase = NULL;
   	CArrayPtr<CContactItem>* item = NULL;

	dBase = CContactDatabase::ReplaceL(iDatabaseFile);
	CleanupStack::PushL(dBase);

	INFO_PRINTF1(_L("Importing Contact....."));
	TBool success = EFalse;
	
	// Importing contact item into vCard format
	
	if ( iOOM != KNullDesC )
		{
		TInt tryCount = 1;
 		TInt err = KErrNone;
 		for ( ;; )
 			{
 			__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
 			TRAP(err, item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EDefault));
 			if ( err==KErrNone )
 				{
 				__UHEAP_RESET;
 				INFO_PRINTF1(_L("OOM testing of CContactDatabase::ImportContactsL Api is done"));
 				INFO_PRINTF2(_L("Total number of contacts imported is %d "), item->Count());
 				break;
 				}
 				
 			if ( err != KErrNoMemory )
 				{
 				__UHEAP_RESET;
 				SetTestStepResult(EFail);
 				break;
 				}
 			tryCount++;
 			}
 			__UHEAP_RESET;
 			dBase->RecoverL();
		}
	else
		{
		if(iEnableImportPBAP)
			{
			// Import with KUidPBAPVCardConvImpl - leaves with KErrNotSupported, as there is no support for PBAP import
			ImportWithPBAP(dBase,readStream,success);
			}
		else
			{
			if(iNumberOfContacts>0)
				{
				TStreamPos pos = 0;
				// Read the contact item one by one into the stream and import 
				for(;;)
					{
					// Importing the contact from readStream
					CArrayPtr<CContactItem>* contactItem = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EImportSingleContact);
					
					TRAPD(completionState,pos = readStream.Source()->TellL(MStreamBuf::ERead));
					if(completionState!=KErrNone || dBase->CountL() >= iNumberOfContacts)
						{
						break;	
						}
					contactItem->ResetAndDestroy();
					}
				}
			else
				{
				item = dBase->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl), readStream, success, CContactDatabase::EDefault);
				}	
			}
		}

	INFO_PRINTF2(_L("Total number of contacts imported is %d "), dBase->CountL());
	
	delete item;
	readStream.Close();
    
    // Cleanup
    CleanupStack::PopAndDestroy(dBase);
    CleanupStack::PopAndDestroy(&fsSession);
    CleanupStack::PopAndDestroy(&readStream);
    fsSession.Close();
	}

// Gets the input from ini file	
void::CTestGenericImportStep::GetInputFromIni()
	{
	iEnableImportPBAP = EFalse;
	GetStringFromConfig(ConfigSection(), KImportFile, iImportFile);
	// Database to Replace
	GetStringFromConfig(ConfigSection(), KDatabaseFile, iDatabaseFile);
	// Number of contacts to import
	GetIntFromConfig(ConfigSection(), KNumberOfContacts, iNumberOfContacts);
	GetStringFromConfig(ConfigSection(),KOOM,iOOM);	
	GetBoolFromConfig(ConfigSection(),KEnableImportPBAP,iEnableImportPBAP);	
	}

/** Import with KUidPBAPVCardConvImpl, which leaves with KErrNotSupported
@param	aDBase The database to import
@param	aReadStream Import stream
@param  aSuccess states the success of import on return
*/
void CTestGenericImportStep::ImportWithPBAP(CContactDatabase* aDBase, RFileReadStream& aReadStream, TBool& aSuccess)
	{
	CArrayPtr<CContactItem>* item = NULL;
	TRAPD(err, item = aDBase->ImportContactsL(TUid::Uid(KUidPBAPVCardConvImpl),aReadStream, aSuccess, CContactDatabase::EImportSingleContact));		
	
	if(err == KErrNotSupported )
		{
		SetTestStepResult(EPass);
		}
	else
		{
		item->ResetAndDestroy();
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
	}

