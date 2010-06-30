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


#include "testcontactoperations.h"

CTestContactOperations::~CTestContactOperations()
/**
 * Destructor
 */
	{
	}

CTestContactOperations::CTestContactOperations()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestContactOperations);
	}

TVerdict CTestContactOperations::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start deletion of Contact items"));

	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestContactOperations::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	DeleteContactsL();
	return TestStepResult();
	}
	
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CTestContactOperations::doTestStepPostambleL()
 	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	
	INFO_PRINTF1(_L("Completed deletion of Contact items"));
	return TestStepResult();
	}

void CTestContactOperations::DeleteContactsL()
	{
	// existing database
   	TPtrC databaseFile(_L("C:contactDB.cdb"));
   	
   	TContactItemId itemId = KErrNotFound;
	CContactDatabase* dBase = NULL;
	CContactIdArray* idArray = NULL;
	
	dBase = CContactDatabase::OpenL(databaseFile);
	CleanupStack::PushL(dBase);
	
	// Set the filter to select all the contact items in the database
	CCntFilter* exportFilter = CCntFilter::NewL();
	CleanupStack::PushL(exportFilter);
	exportFilter->SetContactFilterTypeCard(ETrue);
	dBase->FilterDatabaseL(*exportFilter);
	idArray = exportFilter->iIds;	
	CleanupStack::PushL(idArray);
	
	TInt numberOfContacts;
   	GetIntFromConfig(ConfigSection(), KNumberOfCont, numberOfContacts);
	
	INFO_PRINTF1(_L("Deleting Contacts....."));

	// Delete the contacts one at a time
	for(TInt i=(idArray->Count()-1);i>=0;i--)
		{
		dBase->DeleteContactL((*idArray)[i]);
		}
			
	_LIT(KCount, "The number of contacts in the database is %d \n");
	INFO_PRINTF2(KCount, dBase->CountL());
	
	// Close the contacts
    dBase->CloseContactL(itemId);
	
	// Cleanup
	CleanupStack::Pop(idArray);
    CleanupStack::PopAndDestroy(exportFilter);
    CleanupStack::PopAndDestroy(dBase);
    }

