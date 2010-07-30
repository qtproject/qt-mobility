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
* The test step to construct the views and access the view Api's like CountL(), AtL(), 
* ContactAtL(), FindL(), SortOrder(), ViewPreferences(), AllFieldsLC(), RefineFindView(), 
* GetContactsMatchingCriteria() and GetContactsMatchingPrefix() based on the variable set
* in the ini file. 
* Verify and Validate the values returned by the view API's
*
*/



 
#ifndef TEST_CONTACT_VIEW_ACCESS_STEP_H
#define TEST_CONTACT_VIEW_ACCESS_STEP_H

// System Includes
#include <testexecutestepbase.h>
#include <cntdef.h>
#include <cntviewbase.h>
#include <cntdb.h>
#include <e32property.h>
#include <sbeclient.h>

// User Includes
#include "cntperfserver.h"
#include "clientserver.h"
#include "contactutilitiescollection.h"

_LIT(KViewAccessList,				"viewaccesslist");
_LIT(KTestAtLAPI,					"testatlapi");
_LIT(KTestContactAtLAPI,			"testcontactatlapi");
_LIT(KTestCountLAPI,				"testcountlapi");
_LIT(KTestAllFieldsAPI,				"testallfieldslapi");
_LIT(KTestFindLAPI,					"testfindlapi");
_LIT(KTestSortOrder,				"testsortorder");
_LIT(KTestViewPreferences,			"testviewpreferences");
_LIT(KTestContactsMatchingCriteria,	"testcontactsmatchingcriteria");
_LIT(KTestContactsMatchingPrefix,	"testcontactsmatchingprefix");
_LIT(KTestSearchContactId,			"testsearchcontactid");
_LIT(KTestContactssortorderchange,	"testContactssortorderchange");
_LIT(KRefineFindView,				"refinefindview");
_LIT(KFindWords,					"findwords");
_LIT(KSortOrder,					"sortorder");
_LIT(KErrorCondition,				"errorcondition");
_LIT(KInfoContactFields,			"Contact fields %S");
_LIT(KInvalidData,					"invaliddata");


/**
 * This class supports performing Backup and Restore operations.  
 */	
class CBackupRestoreHelper : public CBase
	{
public:
	static CBackupRestoreHelper* NewL(CTestStep& aTestStep);
	~CBackupRestoreHelper();

	/* Backup and Restore related tests */
	void  StartBackupL();
	void  EndBackupL();
	void  StartRestoreL();
	void  EndRestoreL();

	/* General utility Functions */
	void Validate(const TBool& aValid);
	void Validate(const TInt& aDesiredResult,  const TInt& aActualResult);
	CTestStep& BaseTestStepReference();

private:
	CBackupRestoreHelper(CTestStep& aTestStep);
	void ConstructL();

private:
	TDriveList 			iDriveList;
	conn::CSBEClient* 	iBackupClient;
	CTestStep& 			iBaseTestStep;
	RFs					iFs;
	};
	
/**
 * The test step class is used to construct the views and access the view Api's like CountL(), AtL(), 
 * ContactAtL(), FindL(), SortOrder(), ViewPreferences(), AllFieldsLC(), RefineFindView(), 
 * GetContactsMatchingCriteria() and GetContactsMatchingPrefix() based on the variable set
 * in the ini file. 
 */			
class CTestContactViewAccessStep : public CTestStep
	{
public:
	CTestContactViewAccessStep();
	~CTestContactViewAccessStep();

	// from CTestStep
	TVerdict	doTestStepPreambleL();
	TVerdict	doTestStepL();
	TVerdict	doTestStepPostambleL();

private:
	void	CreateUtilityClassL();
	void	TestViewAPIsL(const RArray<TPtrC>& aSection);
	void	TestContactViewAtL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestContactAtL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestCountL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestFindL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestContactFieldsL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestContactViewPreferencesL(const TPtrC& aSection, CContactViewBase* aView);
	void	TestSortOrderL(const TPtrC& aSection, CContactViewBase* aView);
	CContactDatabase* CreateAndOpenDataBaseL(const TPtrC& aDbName, TBool aCreateDataBase = EFalse);
	CContactDatabase& DatabaseReference();
	CContactViewCollection& ViewCollectionReference();

	void 	AccessViewsL();
	
	void 	TestViewsUnderBackupRestoreL();	
	void 	InitateBackupL();
	void 	StopBackupL();
	void 	InitiateRestoreL();
	void 	StopRestoreL();	
	
private:
	CContactUtilitiesCollection* iContactUtility;
	CContactDatabase* iCntDb;
	CContactViewApiTest* iContactViewTest;
	CContactViewValidation* iViewValidation;
	CActiveScheduler*	iActiveScheduler;
	CContactViewCollection* iViewCollection;
	CBackupRestoreHelper* iBackupHelper;
	TPtrC	iDbName;
	};

_LIT(KTestContactViewAccessStep, "TestContactViewAccessStep");

#endif
