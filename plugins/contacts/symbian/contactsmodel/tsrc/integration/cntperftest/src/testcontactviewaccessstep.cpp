/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testcontactviewaccessstep.h"

/**
 * Constructor
 */
CTestContactViewAccessStep::CTestContactViewAccessStep()
	{
	// Unique loggind Id for the test step
	SetTestStepName(KTestContactViewAccessStep);
	}

/**
 * Destructor
 */
CTestContactViewAccessStep::~CTestContactViewAccessStep()
	{

	if(iViewCollection)
		{
		delete iViewCollection;
		}
	delete iViewValidation;
	delete iContactViewTest;
	delete iContactUtility;
	delete iBackupHelper;
	}

/**
 * Base class pure virtual.
 * @return	EPass or EFail indicating the result of the test step.
 */
TVerdict CTestContactViewAccessStep::doTestStepPreambleL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);

	CTestStep* self = static_cast<CTestStep*>(this);
 	iBackupHelper = CBackupRestoreHelper::NewL(*self);

	// Create utility objects
	CreateUtilityClassL();

	// File from where contacts must be imported
	TPtrC	importFileName;
	TBool	filePresent;
	filePresent = GetStringFromConfig(ConfigSection(), SharedConstants::KFileToBeImported, importFileName);

	TBool	pbapBased = EFalse;
	GetBoolFromConfig(ConfigSection(), SharedConstants::KVCardVersionPBAP, pbapBased);
	if(filePresent)
		{
		iContactUtility->ImportVcardContactsL(importFileName, pbapBased);
		}

	return CTestStep::doTestStepPreambleL();
	}

/**
 * Base class pure virtual.
 * @return EPass or EFail indicating the result of the test step.
 */
TVerdict CTestContactViewAccessStep::doTestStepL()
	{
	TPtrC	listOfViews;
	GetStringFromConfig(ConfigSection(), SharedConstants::KListOfViews, listOfViews);

	/* Get the list of views from ini and construct the view like local view/remote view/named remote view/
	   find view/filtered view/group view/concatenated view or sub view based on the requirement */
	RArray<TPtrC>	sections;
	iContactUtility->TokenizeStringL(listOfViews, sections);
	iContactUtility->ConstructViewsL(sections); //section is destroyed in ConstructViewsL

	AccessViewsL();
	TestViewsUnderBackupRestoreL();

	return TestStepResult();
	}

/**
 * Test the view API's like AtL(), AllFieldsLC(), ContactAtL(), ContactL(), CountL(), SortOrder(),
 * ViewPreferences(), RefineFindView(), ContactsMatchingCriteria() and ContactsMatchingPrefixL()
 * based on the sections given in the ini file.
 * @param aSection ini sections
 */
void CTestContactViewAccessStep::TestViewAPIsL(const RArray<TPtrC>& aSection)
	{
	// Based on the data specified in the ini file, test the view API's
	for ( TInt i = 0; i < aSection.Count(); ++i )
		{		
		TPtrC	viewAccessSection = aSection[i];
		
		// Get the desired view based on the input given from the ini file	
		CContactViewBase*	viewBase = iContactUtility->GetDesiredView(viewAccessSection);	
		CleanupStack::PushL(viewBase);
			
		TBool	testAtLAPI = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestAtLAPI, testAtLAPI);	
		if ( testAtLAPI )
			{			
			// Function to test the AtL() API which returns the contact item id for the specified index
			TestContactViewAtL(viewAccessSection, viewBase);
			}

		TBool	testContactAtLAPI = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestContactAtLAPI, testContactAtLAPI);
		if ( testContactAtLAPI )
			{			
			// Function to test the ContactAtL() API which returns the contact item
			TestContactAtL(viewAccessSection, viewBase);
			}

		TBool	testCountLAPI = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestCountLAPI, testCountLAPI);
		if ( testCountLAPI )
			{
			// Function to test the CountL() which returns the no. of contact items in the view
			TestCountL(viewAccessSection, viewBase);
			}

		TBool	testAllFieldsAPI = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestAllFieldsAPI, testAllFieldsAPI);
		if ( testAllFieldsAPI )
			{
			// Function to test the AllFieldsLC() API which returns the fields of the contact item
			TestContactFieldsL(viewAccessSection, viewBase);
			}

		TBool	testFindLAPI = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestFindLAPI, testFindLAPI);
		if ( testFindLAPI )
			{
			// Function to test the FindL() API which returns the index of the contact item
			TestFindL(viewAccessSection, viewBase);
			}

		TBool	testSortOrder = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestSortOrder, testSortOrder);
		if ( testSortOrder )
			{
			// Function to test the SortOrder() API which returns the view's sort order
			TestSortOrderL(viewAccessSection, viewBase);
			}

		TBool	testViewPreferences = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestViewPreferences, testViewPreferences);
		if ( testViewPreferences )
			{
			// Function to test the ViewPreferences() API which returns the view preferences
			TestContactViewPreferencesL(viewAccessSection, viewBase);
			}

		TBool	refineFindView = EFalse;
		GetBoolFromConfig(viewAccessSection, KRefineFindView, refineFindView);
		if ( refineFindView )
			{
			TInt	index;
			GetIntFromConfig(viewAccessSection, SharedConstants::KIndex, index);
			// Get the desired view based on the input given from the ini file
			CContactFindView* view = iViewCollection->GetDesiredFindView(index);
			TPtrC	searchWords;
			GetStringFromConfig(viewAccessSection, KSearchWords, searchWords);
			iContactViewTest->RefineFindViewL(searchWords, view, iContactUtility);
			}
			
		TBool	testContactsMatchingCriteria = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestContactsMatchingCriteria, testContactsMatchingCriteria);
		// if contactMatchingCriteria is set to true, then get the contacts matching the specified words
		if ( testContactsMatchingCriteria )
			{			
			TPtrC	findWords;
			GetStringFromConfig(viewAccessSection, KFindWords, findWords);
			TPtrC	errorCondition;
			GetStringFromConfig(viewAccessSection, KErrorCondition, errorCondition);
			RPointerArray <CViewContact> matchContactArray = iContactViewTest->GetContactsMatchingCriteriaL(findWords, viewBase, iContactUtility, errorCondition);
			CleanupClosePushL(matchContactArray);
			if(matchContactArray.Count() != 0)
				{
				iViewValidation->ValidateCountL(viewAccessSection, matchContactArray.Count());
				}			
			CleanupStack::PopAndDestroy(&matchContactArray);
			}

		TBool	testContactsMatchingPrefix = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestContactsMatchingPrefix, testContactsMatchingPrefix);

		// if contactsMatchingPrefix is set to true, then get the contacts where the contacts matches with the prefix
		if ( testContactsMatchingPrefix )
			{			
			TPtrC	errorCondition;
			GetStringFromConfig(viewAccessSection, KErrorCondition, errorCondition);
			TPtrC	searchWords;
			GetStringFromConfig(viewAccessSection, KSearchWords, searchWords);
			RPointerArray <CViewContact> matchContactArray = iContactViewTest->GetContactsMatchingPrefixL(searchWords, viewBase, iContactUtility, errorCondition);
			CleanupClosePushL(matchContactArray);
			if(matchContactArray.Count() != 0)
				{
				iViewValidation->ValidateCountL(viewAccessSection, matchContactArray.Count());	
				}			
			CleanupStack::PopAndDestroy(&matchContactArray);
			}
		TBool	testSearchContact = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestSearchContactId, testSearchContact);
		
		// if testSearchContact is set to true, then get the contacts which is having that search word and verify its index
		if(testSearchContact)
			{
			TPtrC	errorCondition;			
			GetStringFromConfig(viewAccessSection, KErrorCondition, errorCondition);
			viewBase = iContactUtility->GetDesiredView(viewAccessSection);
			TPtrC	searchWords;
			GetStringFromConfig(viewAccessSection, KSearchWords, searchWords);
			RPointerArray <CViewContact> matchContactArray = iContactViewTest->GetContactsMatchingCriteriaL(searchWords, viewBase, iContactUtility, errorCondition);
			CleanupClosePushL(matchContactArray);			
			if(matchContactArray.Count()!=0 )
				{
				TInt	index = iContactViewTest->FindL(matchContactArray[0]->Id(), viewBase, errorCondition);
				iViewValidation->ValidateFindL(viewAccessSection, index);
				}
			else
				{
				ERR_PRINTF2(KContactNotFound, matchContactArray.Count());
				SetTestStepResult(EFail);
				}			
			CleanupStack::PopAndDestroy(&matchContactArray);
			}
		
		TBool	testContactssortorderchange = EFalse;
		GetBoolFromConfig(viewAccessSection, KTestContactssortorderchange, testContactssortorderchange);	
		
		// if testContactssortorderchange is set to true, then change the sort order of the desired view
		if ( testContactssortorderchange )
			{				
			iContactViewTest->TestChangeSortOrderL(viewAccessSection, iContactUtility);			
			}
		CleanupStack::Pop(viewBase);
		}
	}

/**
 * Verify and validate the Contact item id returned by the AtL() API
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestContactViewAtL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TPtrC	errorCondition;
	GetStringFromConfig(aSection, KErrorCondition, errorCondition);
	// Get the required data for AtL() API from the ini file
	TInt	index = iContactViewTest->GetAtLApiDataFromIni(aSection);
	// Call the AtL() API by passing the index and the desired view
	TInt	desiredContactItemId = iContactViewTest->AtL(index, aView, errorCondition);
	// validate the returned contact item id with the expected contact item id
	if(desiredContactItemId != -1)
		{
		iViewValidation->ValidateAtLApi(desiredContactItemId, aSection);
		}
	}

/**
 * Verify and Validate the contact item returned by the ContactAtL() API
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestContactAtL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TInt	index = iContactViewTest->GetContactAtLApiDataFromIni(aSection);
	CViewContact*	desiredContactItem = iContactViewTest->ContactAtL(index, aView);
	if(desiredContactItem != NULL)
		{
		CleanupStack::PushL(desiredContactItem);
		iViewValidation->ValidateContactAtLApi(desiredContactItem, aSection);
		CleanupStack::PopAndDestroy(desiredContactItem);
		}
	}

/**
 * Verify and validate the count returned by the CountL() API
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestCountL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TInt count = 0;
	TRAPD( err, count = aView->CountL());
	if ( err != KErrNone )
		{
		SetTestStepError(err);
		SetTestStepResult(EFail);
		}
	else
		{
		iViewValidation->ValidateCountL(aSection, count);
		}	
	}

/**
 * Verify and validate the index returned by the FindL() API
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestFindL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TPtrC	errorCondition;
	GetStringFromConfig(aSection, KErrorCondition, errorCondition);
	TInt	id = iContactViewTest->GetFindLApiDataFromIni(aSection);
	TInt	index = iContactViewTest->FindL(id, aView, errorCondition);
	if(index > -1)
		{
		iViewValidation->ValidateFindL(aSection, index);
		}	
	}

/**
 * Verify and validate the fields returned by the AllFieldsLC() API.
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestContactFieldsL(const TPtrC& aSection, CContactViewBase* aView)
	{
	HBufC* fields = NULL;
	TBool invalidData = EFalse;	
	GetBoolFromConfig(aSection, KInvalidData, invalidData);	
	if(invalidData)
		{
		//If ivalid data is passed then in udeb mode it is panic but in urel mode it is returning NULL
		#ifndef _DEBUG						
		TRAPD( err, fields = iContactViewTest->GetAllFieldsLDataFromIniL(aSection, aView));
		if ( err != KErrNone )
			{
			SetTestStepError(err);
			SetTestStepResult(EFail);
			}
		else
			{
			if(fields != NULL)
				{
				// Append fields one by one to the fieldArray
				RArray<TPtrC>	fieldArray;
				iContactUtility->TokenizeStringL(*fields, fieldArray);
				iViewValidation->ValidateAllFieldsL(aSection, fieldArray, aView, iContactViewTest);
				}
			else
				{
				INFO_PRINTF2(KInfoContactFields,&fields);
				SetTestStepResult(EFail);
				}
			}			
		#else
			SetTestStepResult(EFail);
		#endif
		}
	else
		{
		TRAPD( err, fields = iContactViewTest->GetAllFieldsLDataFromIniL(aSection, aView));
			if ( err != KErrNone )
				{
				SetTestStepError(err);
				SetTestStepResult(EFail);
				}
			else
				{
				if(fields != NULL)
					{
					// Append fields one by one to the fieldArray
					RArray<TPtrC>	fieldArray;
					iContactUtility->TokenizeStringL(*fields, fieldArray);
					iViewValidation->ValidateAllFieldsL(aSection, fieldArray, aView, iContactViewTest);
					}
				else
					{
					INFO_PRINTF2(KInfoContactFields,&fields);
					SetTestStepResult(EFail);
					}
				}
			}
	}

/**
 * Verify and validate the contact view preferences
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestContactViewPreferencesL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TContactViewPreferences	desiredViewPreferences = iContactViewTest->ContactViewPreferences(aView);
	iViewValidation->ValidateViewPreferencesL(aSection, desiredViewPreferences, iContactUtility);
	}

/**
 * Verify and validate the sort order returned by the SortOrderL() API
 * @param aSection ini section
 * @param aView desired view
 */
void CTestContactViewAccessStep::TestSortOrderL(const TPtrC& aSection, CContactViewBase* aView)
	{
	TPtrC	errorCondition;
	GetStringFromConfig(aSection, KErrorCondition, errorCondition);
	// Get the sort order for the desired view
	RContactViewSortOrder	contactViewSortOrder = iContactViewTest->GetSortOrderL(aView, errorCondition);
	// Get the uid's of the retrieved sort order
	TPtrC	sortOrder;
	GetStringFromConfig(aSection, KSortOrder, sortOrder);
	RContactViewSortOrder	viewSortOrder =	iContactUtility->ConvertStringToSortOrderL(sortOrder);	
	// iterate through the Uid's and validate them
	for ( TInt i = 0; i < contactViewSortOrder.Count(); ++i )	
		{
		TUid	expectedUid = viewSortOrder[i];
		TUid	uid = contactViewSortOrder[i];		
		iViewValidation->ValidateSortOrderL(uid, expectedUid);
		}
	}

/**
 * Create objects of CContactUtilitiesCollection, CContactViewApiTest and CContactViewValidation
 */
void CTestContactViewAccessStep::CreateUtilityClassL()
	{
	TBool	createDataBase = ETrue;
	GetBoolFromConfig(ConfigSection(), SharedConstants::KCreateDataBase, createDataBase);
	GetStringFromConfig(ConfigSection(), SharedConstants::KDbName, iDbName);

	iCntDb = CreateAndOpenDataBaseL(iDbName, createDataBase);
	iViewCollection = CContactViewCollection::NewL();
	// Create Utility class object, to export the contact from database
	CTestStep* self = static_cast<CTestStep*>(this);
	CleanupStack::PushL(self);
	iContactUtility = CContactUtilitiesCollection::NewL(*self, DatabaseReference(), ViewCollectionReference());
	iContactViewTest = CContactViewApiTest::NewL(self);
	iViewValidation = new(ELeave) CContactViewValidation(self);
	CleanupStack::Pop(self);
	}

/**
 * Return the database reference
 * @return iCntDb 
 */
CContactDatabase& CTestContactViewAccessStep::DatabaseReference()
	{
	return *iCntDb;
	}

/**
 * Return the viewcollection reference
 * @return  iViewCollection 
 */
CContactViewCollection& CTestContactViewAccessStep::ViewCollectionReference()
	{
	return *iViewCollection;
	}


/**
 * Create and open the contact database
 * @param aDbName database to be created/ opened
 * @return CContactDatabase*
 */
CContactDatabase* CTestContactViewAccessStep::CreateAndOpenDataBaseL(const TPtrC& aDbName, TBool aCreateDataBase)
	{
	HBufC*	dbName = HBufC::NewLC(aDbName.Length());
	dbName->Des().Copy(aDbName);
	CContactDatabase*	cntDb = NULL;
	if( aCreateDataBase )
		{
		TInt	err = 0;
		// Replace the existing database and opens it
		if( aDbName != KNullDesC() )
			{
			TRAP(err, cntDb = CContactDatabase::ReplaceL(dbName->Des()));
			}
		else //if database name is not given then create and open the default DB
			{
			TRAP(err, cntDb = CContactDatabase::ReplaceL());
			}

		if( err != KErrNone )
			{
			ERR_PRINTF2(KErrInCreateDataBase, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	else
		{
		TRAPD(err, cntDb = CContactDatabase::OpenL(dbName->Des()));
		if( err != KErrNone )
			{
			ERR_PRINTF2(KErrInOpen, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	CleanupStack::PopAndDestroy(dbName);
	return	cntDb;
	}

/**
 * Base class pure virtual.
 * @return		EPass or EFail indicating the result of the test step.
 */
TVerdict CTestContactViewAccessStep::doTestStepPostambleL()
	{
	delete iViewCollection;
	iViewCollection = NULL;
	// Delete the database if the variable deleteDatabase is set to true
	TBool	deleteDataBase = EFalse;
	GetBoolFromConfig(ConfigSection(), SharedConstants::KDeleteDataBase, deleteDataBase);
	delete iCntDb;
	iContactUtility->DeleteDataBase(deleteDataBase, iDbName);
	return CTestStep::doTestStepPostambleL();
	}

/**
 * Access Contact View Apis and validate their behaviour
 */
void CTestContactViewAccessStep::AccessViewsL()
	{
	TPtrC	viewAccessList;
	GetStringFromConfig(ConfigSection(), KViewAccessList, viewAccessList);

	/* if the view access list is defined in the ini then test the views api's like CountL(), AtL(), ContactAtL(),
	   FindL(), SortOrder(), ViewPreferences(), AllFieldsLC(), RefineFindView(), GetContactsMatchingCriteria()
	   and GetContactsMatchingPrefix() for the desired view */
	if ( viewAccessList != KNullDesC() )
		{
		RArray<TPtrC>	accessList;
		CleanupClosePushL(accessList);
		iContactUtility->TokenizeStringL(viewAccessList, accessList);
		TestViewAPIsL(accessList);
		CleanupStack::PopAndDestroy(&accessList);
		}
	}


/**
 * Access Contact View Apis under backup restore conditions
 */
void CTestContactViewAccessStep::TestViewsUnderBackupRestoreL()
	{
	_LIT(KViewAccessUnderBackupRestore, "ViewAccessUnderBackupRestore");
	TBool	viewAccessUnderBackupRestore = EFalse;
	GetBoolFromConfig(ConfigSection(), KViewAccessUnderBackupRestore, viewAccessUnderBackupRestore);
	
	if(!viewAccessUnderBackupRestore)
		{
		return;	
		}

	InitateBackupL();
	AccessViewsL();

	StopBackupL();
	AccessViewsL();

	InitiateRestoreL();
	AccessViewsL();

	StopRestoreL();
	AccessViewsL();
	}

/**
 * Initate a system wide backup operation
 */
void CTestContactViewAccessStep::InitateBackupL()
	{
	iBackupHelper->StartBackupL();
	}
/**
 * Stop the system wide backup operation
 */
void CTestContactViewAccessStep::StopBackupL()
	{
	iBackupHelper->EndBackupL();
	}
/**
 * Initate a system wide restore operation
 */
void CTestContactViewAccessStep::InitiateRestoreL()
	{
	iBackupHelper->StartRestoreL();
	}
/**
 * Stop the system wide restore operation
 */
void CTestContactViewAccessStep::StopRestoreL()
	{
	iBackupHelper->EndBackupL();
	}

/**
 * Initate a system wide restore operation
 */
CBackupRestoreHelper* CBackupRestoreHelper::NewL(CTestStep& aTestStep)
	{
	CBackupRestoreHelper* self = new(ELeave) CBackupRestoreHelper(aTestStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
 * Class CBackupRestoreHelper - Constructor
 * @param aTestStep - Reference to current test step, used to read data from the ini file
 */
CBackupRestoreHelper::CBackupRestoreHelper(CTestStep& aTestStep)
:iBaseTestStep(aTestStep)
	{
	}

/**
 * Class CBackupRestoreHelper - second level constructor
 */
void CBackupRestoreHelper::ConstructL()
	{
	/** Initialise the drive list to empty and then get drive list data from
	 File Server.
	 Required before Backup and Restore testing
	 */
	iFs.Connect();
	iDriveList.FillZ();
	User::LeaveIfError(iFs.DriveList(iDriveList));
	iBackupClient = conn::CSBEClient::NewL();
	}

/**
 * Class CBackupRestoreHelper - destructor
 */
CBackupRestoreHelper::~CBackupRestoreHelper()
	{
	delete iBackupClient;
	iFs.Close();
	}

/**
 * Uses the Secure Backup Engine API  and starts a backup.
 */
void CBackupRestoreHelper::StartBackupL()
	{
	iBackupClient->SetBURModeL(iDriveList, conn::EBURBackupFull, conn::EBackupBase);
	}

/**
 * Uses the Secure Backup Engine API  and ends backup.
 */
void CBackupRestoreHelper::EndBackupL()
	{
	iBackupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
	}

/**
 * Uses the Secure Backup Engine API  and starts a restore.
 */
void CBackupRestoreHelper::StartRestoreL()
	{
	iBackupClient->SetBURModeL(iDriveList, conn::EBURRestoreFull, conn::EBackupBase);
	}

/**
 * Uses the Secure Backup Engine API  and ends restore.
 */
void CBackupRestoreHelper::EndRestoreL()
	{
	/** Use the Secure Backup Engine API to "end" a restore. */
	iBackupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
	}


/**
 * Reference to TestStep, used for retreiving ini data, printing operations etc.. 
 */
CTestStep& CBackupRestoreHelper::BaseTestStepReference()
	{
	return(iBaseTestStep);
	}
