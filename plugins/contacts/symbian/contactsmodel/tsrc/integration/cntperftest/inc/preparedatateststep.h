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




#ifndef __PREPARE_DATA_TEST_STEP_H__
#define __PREPARE_DATA_TEST_STEP_H__

// System Includes
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <cntfldst.h>
#include <cntfilt.h>
#include <e32math.h>

// User Includes
#include "addmodifydeletecontactfields.h"
#include "testcontactviewdatabaseutilitiesstep.h"


/**
 * This class supports addition of contact items and groups in the database basically from view construction
 * point of view. While adding contact items, the following things are taken into consideration
 * -- Num of desired Sortable contacts specific to Sort order
 * -- Num of Unsortable Contacts specific to Sort order
 * -- Num of Contacts with desired Contact View Filter
 * -- Num of contacts with desired Strings
 * -- Num of Groups and Num of contacts in each group
 *
 */	
class CPrepareDataTestStep : public CTestContactViewDatabaseUtilitiesStep, public TAddModifyDeleteContactFields
	{
	public:
		CPrepareDataTestStep();
		~CPrepareDataTestStep();
		TVerdict doTestStepPreambleL();
		TVerdict doTestStepL();
		TVerdict doTestStepPostambleL();
	
	private:
		void AddContactsInDatabaseL();
		void ImportContactsL();		
		void UpdateContactFieldsSpecificToSortOrderL(const TPtrC& aSortOrderSection);
		void AddContactFieldL(CContactItem& aContact, const RContactViewSortOrder& aSortOrder, TBool aData = EFalse);		
	
		void IterateThroAllSortOrderSectionsAndUpdateContactsInDatabaseL();
		void IterateThroAllSortOrderSectionsAndUpdateContactsL();
	
		void  AddTypicalContactForFilterL(CContactItem& aContact, TInt aBitWiseFilter);	
		TBool CheckAllTheFieldsExists(const CContactItem& aContact, const RContactViewSortOrder& aSortOrder);
		void  IterateThroAllFilterViewsSectionsAndUpdateL();
		void  UpdateContactsInDatabaseAsPerFilteredViewRequirementsL(const TPtrC& aFilteredViewSection);

		void RetrieveContactsSpecificToSortOrderL(const RContactViewSortOrder& aSortOrder, RArray<TContactItemId>& aContactSpecificToSorder);
		void IterateThroAllMatchingStringSectionsAndUpdateL();
		void UpdateContactsInDatabaseAsPerMatchingStringsL(const TPtrC& aMatchingStringSection);
		void AddMatchingStringToContactL(CContactItem& aContact, TUid aUid, const TPtrC& aDesiredMatchingString);
		void UpdateContactToSpecificStringL();
		void UpdateContactFieldsToDesireStingL(const TPtrC& aMatchingStringSection);

	private:
		CActiveScheduler* 				iScheduler;
		CContactDatabase*				iDb;
		RArray <TContactItemId>  		iListOfContactItemsInDatabase;
		TInt64 							iRandSeed;	
	};

_LIT(KPrepareDataTestStep,"PrepareDataTestStep");
#endif



